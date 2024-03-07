<script lang="ts">
  export let n_tiempos: number;
  export let salidas: string[];
  export let onUpdate: (entries: Map<string, boolean[]>) => void;

  export let entries: Map<string, boolean[]>;

  function newEntry(
    e: Event & {
      currentTarget: EventTarget & HTMLInputElement;
    }
  ) {
    const target = e.currentTarget;
    const { value } = target;

    target.value = "";

    if (entries.has(value)) {
      return;
    }

    entries.set(
      value,
      salidas.map((_) => false)
    );
    entries = new Map([...entries].sort());

    onUpdate(new Map([...entries]));
  }

  function deleteEntry(entry: string) {
    entries.delete(entry);
    entries = entries;

    onUpdate(new Map([...entries]));
  }

  function onTimeChange(
    e: Event & {
      currentTarget: EventTarget & HTMLInputElement;
    },
    prev: string
  ) {
    const target = e.currentTarget;
    const { value } = target;

    if (!value || entries.has(value)) {
      target.value = prev;
      return;
    }

    entries.set(value, entries.get(prev));
    entries.delete(prev);
    entries = new Map([...entries].sort());

    onUpdate(new Map([...entries]));
  }

  function onOutChange(checked: boolean, index: number, entry: string) {
    let arr = entries.get(entry);

    arr[index] = checked;

    if (!arr.includes(true, 1)) {
      arr[0] = false;
    }

    entries.set(entry, arr);
    entries = entries;

    onUpdate(new Map([...entries]));
  }
</script>

<div class="table max-w-fit">
  <div class="filler col-time">
    <button class="btn-delete">x</button>
  </div>
  <div class="filler col-time">
    <input type="time" value="00:00" />
  </div>

  {#each salidas as s}
    <span class="out mx-auto">{s}</span>
  {/each}

  {#each [...entries] as [k, v]}
    <div>
      <button class="btn-delete" on:click={() => deleteEntry(k)}>x</button>
    </div>
    <div class="col-time">
      <input type="time" value={k} on:change={(e) => onTimeChange(e, k)} />
    </div>

    {#each v as v, i}
      <div class="grid-center">
        <input
          type="checkbox"
          bind:checked={v}
          on:change={({ currentTarget: { checked } }) =>
            onOutChange(checked, i, k)}
        />
      </div>
    {/each}
  {/each}

  {#if entries.size < n_tiempos}
    <div class="filler col-time">
      <button class="btn-delete">x</button>
    </div>
    <div class="col-time">
      <input type="time" class="input-new-entry" on:change={newEntry} />
    </div>
  {/if}
</div>

<style>
  input[type="time"] {
    border: none;
    width: fit-content;
  }

  input[type="checkbox"] {
    appearance: none;
    width: 1rem;
    height: 1rem;
    border: 1px solid #0003;
    border-radius: 4px;
  }

  input[type="checkbox"]:checked {
    position: relative;
    appearance: none;
    width: 1rem;
    height: 1rem;
    border: 1px solid #0003;
  }

  input[type="checkbox"]:checked::after {
    content: "";
    position: absolute;
    left: 50%;
    top: 50%;
    transform: translate(-50%, -50%);
    width: 0.6rem;
    height: 0.6rem;
    background: hsl(140, 54%, 48%);
    border: 1px solid #0002;
    border-radius: 2px;
  }

  input.input-new-entry {
    width: 100%;
  }

  .mx-auto {
    margin: 0 auto;
  }

  .grid-center {
    display: grid;
    place-items: center;
  }

  .table {
    display: grid;
    grid-template-columns: fit-content(1px) fit-content(1px) 1fr 1fr 1fr 1fr 1fr 1fr 1fr;
  }

  .filler {
    pointer-events: none;
    visibility: hidden;
  }

  .col-time {
    padding: 0.1rem 0.4rem;
    border-left: 1px solid #0002;
    border-right: 1px solid #0002;
  }

  .out {
    font-family: monospace;
  }

  .btn-delete {
    background: none;
    border: none;
    color: #d33;
    font-weight: 600;
  }
</style>
