<script lang="ts">
  import { onMount } from "svelte";
  import ModoManual from "./ModoManual.svelte";
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
  const salidas = ["bomba", "1", "2", "3", "4", "5", "6"];

  let tablaTiempos: Map<string, boolean[]>[] = [...Array(dias.length)].map(
    () => new Map()
  );

  let selectedTab = 0;
  let copyingId = -1;
  let manualEnabled = false;

  onMount(() => {
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

    fetch("/manual_activado").then(async (res) => {
      const data: { val: boolean } = await res.json();
      manualEnabled = data.val;
    });
  });

  function onUpload() {
    fetch("/tiempos", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(
        tablaTiempos.map((i) =>
          [...i.entries()].map(([k, v]) => {
            let [h, m] = k.split(":");

            return [parseInt(h), parseInt(m), ...v.map((i) => +i)];
          })
        )
      ),
    });
  }

  function clickManual() {
    manualEnabled = !manualEnabled;

    fetch(manualEnabled ? "/activar_manual" : "/desactivar_manual", {
      method: "POST",
    });
  }
</script>

<main>
  <div>
    <TabsDias {dias} bind:selected={selectedTab} />

    <div class="btns">
      <button on:click={() => (copyingId = selectedTab)}>Copiar</button>
      <button
        on:click={() => {
          if (copyingId === -1) {
            return;
          }

          tablaTiempos[selectedTab] = new Map(
            [...tablaTiempos[copyingId]].map(([k, v]) => [k, [...v]])
          );

          copyingId = -1;
        }}>Pegar</button
      >
      <div class="flex-1" />
      <button
        class="btn-manual"
        class:enabled={manualEnabled}
        on:click={clickManual}>Manual</button
      >
      <button class="btn-upload" on:click={onUpload}>Subir</button>
    </div>
    <span>{copyingId !== -1 ? `Copiando: ${dias[copyingId]}` : ""}</span>

    <div class="divider" />
    {#if !manualEnabled}
      {#each dias as _, i}
        <div style:display={selectedTab === i ? "block" : "none"}>
          <TablaTiempos
            {n_tiempos}
            {salidas}
            entries={tablaTiempos[i]}
            onUpdate={(entries) => (tablaTiempos[i] = entries)}
          />
        </div>
      {/each}
    {:else}
      <ModoManual {salidas} />
    {/if}
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

  .flex-1 {
    flex: 1;
  }

  span {
    font-size: 0.8rem;
    text-transform: capitalize;
    display: block;
    margin-top: 8px;
  }

  .btns {
    display: flex;
    align-items: center;
    gap: 4px;
    margin-top: 16px;
  }

  .btns button {
    background: hsl(0, 0%, 90%);
    -webkit-tap-highlight-color: hsla(140, 68%, 46%, 0.3);
    border: none;
    padding: 0.6rem 0.7rem;
    font-weight: 500;
    border-radius: 4px;
    cursor: pointer;
  }

  .btns button:hover {
    background: hsl(0, 0%, 85%);
  }

  .btns button:active {
    background: hsl(0, 0%, 80%);
  }

  .btns .btn-upload {
    background: hsl(140, 68%, 46%);
    color: #fff;
  }

  .btns .btn-upload:hover {
    background: hsl(140, 68%, 42%);
  }

  .btns .btn-upload:active {
    background: hsl(140, 68%, 34%);
  }

  .btns .btn-manual {
    -webkit-tap-highlight-color: #0000;
    transition: 0.2s;
  }

  .btns .btn-manual:not(.enabled) {
    color: hsl(0, 0%, 50%);
    background: #fff;
    border: 2px solid hsl(0, 0%, 90%);
  }

  .btns .btn-manual.enabled {
    color: hsl(52, 90%, 30%);
    background: hsl(52, 100%, 85%);
    border: 2px solid hsl(52, 90%, 40%);
  }
</style>
