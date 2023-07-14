const char HTML_INDEX[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <title>Tiempos ESP32</title>
  </head>
  <body>
    <pre style="font-family: monospace"></pre>
  </body>
  <script>
    const txt = document.querySelector("pre");

    fetch("/tiempos")
      .then(async (response) => {
        const data = await response.json();

        txt.textContent = JSON.stringify(data, null, 2);
      });
  </script>
</html>
)=====";
