<script lang="ts">
  export let dias: string[];
  export let selected = 0;
</script>

<ul>
  {#each dias as d, i}
    <li>
      <button class:selected={i === selected} on:click={() => (selected = i)}>
        {d.slice(0, 3)}
      </button>
    </li>
  {/each}
</ul>

<style>
  ul {
    display: flex;
    overflow-x: auto;
  }

  li {
    flex: 1;
  }

  button {
    width: 100%;
    background: none;
    border: none;
    padding: 0.4rem 0.4rem calc(0.4rem - 2px);
    font-weight: 500;
    text-transform: uppercase;
  }

  button.selected {
    border-bottom: 2px solid #1bc454;
  }
</style>
