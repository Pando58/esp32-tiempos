<script lang="ts">
  export let n_tiempos: number;
  export let n_salidas: number;
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
      [...Array(n_salidas)].map((_) => false)
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

    if (entries.has(value)) {
      target.value = prev;
      return;
    }

    entries.set(value, entries.get(prev));
    entries.delete(prev);
    entries = new Map([...entries].sort());

    console.log(entries);

    onUpdate(new Map([...entries]));
  }

  function onOutChange(checked: boolean, index: number, entry: string) {
    let arr = entries.get(entry);
    arr[index] = checked;
    entries.set(entry, arr);
    entries = entries;

    onUpdate(new Map([...entries]));
  }
</script>

<div class="table">
  <div class="filler col-time">
    <button class="btn-delete">x</button>
  </div>
  <div class="filler col-time">
    <input type="time" value="00:00" />
  </div>

  {#each [...Array(n_salidas)].map((_, i) => i) as s}
    <span class="out mx-auto">out{s}</span>
  {/each}

  {#each entries as [k, v]}
    <div>
      <button class="btn-delete" on:click={() => deleteEntry(k)}>x</button>
    </div>
    <div class="col-time">
      <input type="time" value={k} on:change={(e) => onTimeChange(e, k)} />
    </div>

    {#each v as v, i}
      <input
        type="checkbox"
        class="mx-auto"
        bind:checked={v}
        on:change={({ currentTarget: { checked } }) =>
          onOutChange(checked, i, k)}
      />
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

  input.input-new-entry {
    width: 100%;
  }

  .mx-auto {
    margin: 0 auto;
  }

  .table {
    display: grid;
    grid-template-columns: fit-content(1px) fit-content(1px) 1fr 1fr 1fr;
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