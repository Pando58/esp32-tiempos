<script lang="ts">
  let counter = 0;
</script>

<main>
  <div class="container">
    <div>
      <h2>{counter}</h2>
      <button on:click={() => counter--}><span>{counter - 1}</span></button>
      <button on:click={() => counter++}><span>{counter + 1}</span></button>
    </div>
  </div>
</main>

<style>
  * {
    font-family: monospace;
    font-size: 2rem;
    font-weight: 600;
  }

  .container {
    height: 100vh;
    display: grid;
    place-items: center;
  }

  .container div {
    text-align: center;
  }

  button {
    padding: 0.2rem;
  }

  button span {
    display: inline-block;
    min-width: 2ch;
  }
</style>
