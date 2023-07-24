<script lang="ts">
  import { onMount } from "svelte";

  export let salidas: string[];

  let values = salidas.map((_) => false);

  $: console.log(values);

  onMount(() => {
    fetch("/manual").then(async (res) => {
      const data: boolean[] = await res.json();
      values = data;
    });
  });

  function changeEntry(index: number, value: boolean) {
    fetch("/manual", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({
        entry: index,
        value,
      }),
    });
  }
</script>

<div class="table">
  {#each salidas as s}
    <span class="out mx-auto">{s}</span>
  {/each}

  {#each values as _, i}
    <div class="grid-center">
      <input
        type="checkbox"
        bind:checked={values[i]}
        on:change={(e) => changeEntry(i, e.currentTarget.checked)}
      />
    </div>
  {/each}
</div>

<style>
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

  .grid-center {
    display: grid;
    place-items: center;
  }

  .table {
    display: grid;
    grid-template-columns: 1fr 1fr 1fr;
  }

  .out {
    margin: 0 auto;
    padding: 0.4rem 0;
    font-family: monospace;
  }
</style>
