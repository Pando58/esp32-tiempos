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
      <button class="btn-upload" on:click={onUpload}>Subir</button>
    </div>
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
    background: #ddd;
    border: none;
    padding: 0.3rem 0.4rem;
    font-weight: 500;
    border-radius: 4px;
  }

  .btns .btn-upload {
    background: #25c45a;
    color: #fff;
  }
</style>
