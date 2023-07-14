<script lang="ts">
  import TablaTiempos from "./TablaTiempos.svelte";
  import TabsDias from "./TabsDias.svelte";

  const dias = [
    "lunes",
    "martes",
    "miércoles",
    "jueves",
    "viernes",
    "sábado",
    "domingo",
  ];

  const n_tiempos = 10;
  const n_salidas = 3;

  let tablaTiempos: Map<string, boolean[]>[] = [...Array(dias.length)].map(
    () => new Map()
  );

  let selectedTab = 0;
  let copyingId = -1;

  fetch("/tiempos")
    .then(async (res) => {
      const data: {
        hora: number;
        minuto: number;
        salidas: boolean[];
      }[][] = await res.json();

      tablaTiempos = data.map(
        (i) =>
          new Map(
            i
              .filter(({ hora }) => hora !== -1)
              .map(({ hora, minuto, salidas }) => {
                let k = `${hora.toString().padStart(2, "0")}:${minuto
                  .toString()
                  .padStart(2, "0")}`;

                return [k, salidas];
              })
          )
      );
    })
    .catch((err) => {
      console.error(err);
    });
</script>

<main>
  <div>
    <TabsDias {dias} bind:selected={selectedTab} />

    <button on:click={() => (copyingId = selectedTab)}>Copiar</button>
    <button
      on:click={() => {
        tablaTiempos[selectedTab] = new Map(
          [...tablaTiempos[copyingId]].map(([k, v]) => [k, [...v]])
        );

        copyingId = -1;
      }}>Pegar</button
    >
    <span>{copyingId !== -1 ? `Copiando: ${dias[copyingId]}` : ""}</span>

    <div class="divider" />
    {#each dias as _, i}
      <div style:display={selectedTab === i ? "block" : "none"}>
        <TablaTiempos
          {n_tiempos}
          {n_salidas}
          entries={tablaTiempos[i]}
          onUpdate={(entries) => (tablaTiempos[i] = entries)}
        />
      </div>
    {/each}
  </div>
</main>

<style>
  main {
    padding: 2rem;
    max-width: 1280px;
    margin: 0 auto;
  }

  .divider {
    height: 2rem;
  }

  button {
    background: #ddd;
    border: none;
    padding: 0.3rem 0.4rem;
    margin: 1rem 0;
  }

  span {
    margin-left: 0.3rem;
    font-size: 0.8rem;
    text-transform: capitalize;
  }
</style>
