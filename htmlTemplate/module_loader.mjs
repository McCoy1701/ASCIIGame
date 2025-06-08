export async function initModuleLoader(Module) {
  console.log("We are OUT -- OF -- THE -- OBJECT -- INITIALIZATION -- ! ");
  console.log("");
  setTimeout(() => {
    /* -----
  TEST YOUR CODE HERE
  ----- */
    colorLoop(Module, true);
  }, 1); // delay to start
}

// This makes it globally available so template.html can use it
window.initModuleLoader = initModuleLoader;

export function colorLoop(Module, random = false) {
  let colors = [
    0xd1b187, 0xc77b58, 0xae5d40, 0x79444a, 0x4b3d44, 0xba9158, 0x927441,
    0x4d4539, 0x77743b, 0xb3a555, 0xd2c9a5, 0x8caba1, 0x4b726e, 0x574852,
    0x847875, 0xab9b8e,
  ];
  let index = 0;

  function changeColor() {
    if (random) {
      colors = shuffle(colors);
    }
    Module._g_ChangeColor(colors[index]);
    index = (index + 1) % colors.length;
    // recursively call itself... :)
    setTimeout(changeColor, 1000);
  }

  changeColor();
}

export function shuffle(array) {
  let currentIndex = array.length,
    randomIndex;

  // While there remain elements to shuffle.
  while (currentIndex != 0) {
    // Pick a remaining element.
    randomIndex = Math.floor(Math.random() * currentIndex);
    currentIndex--;

    // And swap it with the current element.
    [array[currentIndex], array[randomIndex]] = [
      array[randomIndex],
      array[currentIndex],
    ];
  }

  return array;
}
