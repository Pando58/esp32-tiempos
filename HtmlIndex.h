const char HTML_INDEX[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <title>Tiempos ESP32</title>
  </head>
  <body>
    <h1>val: <span></span></h1>
  </body>
  <script>
    const txt = document.querySelector("h1 span");

    fetch("/tiempos")
      .then(async (response) => {
        const data = await response.json();

        console.log(data);
        txt.innerHTML = data.val;
      });
  </script>
</html>
)=====";
