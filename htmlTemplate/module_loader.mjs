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

// Now we can define functions

export function colorLoop(Module, random = false) {
  // Organized color palette with descriptive groupings
  const colorPalette = {
    blues: [0x172038, 0x253a5e, 0x3c5e8b, 0x4f8fba, 0x73bed3, 0xa4dddb],
    greens: [0x19332d, 0x25562e, 0x468232, 0x75a743, 0xa8ca58, 0xd0da91],
    browns: [0x4d2b32, 0x7a4841, 0xad7757, 0xc09473, 0xd7b594, 0xe7d5b3],
    oranges: [0x341c27, 0x602c2c, 0x884b2b, 0xbe772b, 0xde9e41, 0xe8c170],
    reds: [0x241527, 0x411d31, 0x752438, 0xa53030, 0xcf573c, 0xda863e],
    purples: [0x1e1d39, 0x402751, 0x7a367b, 0xa23e8c, 0xc65197, 0xdf84a5],
    grays: [
      0x090a14, 0x10141f, 0x151d28, 0x202e37, 0x394a50, 0x577277, 0x819796,
      0xa8b5b2, 0xc7cfcc, 0xebede9,
    ],
  };

  // Flatten all colors into a single array
  let colors = Object.values(colorPalette).flat();
  let index = 0;

  function changeColor() {
    if (random) {
      // Pick a completely random color each time
      const randomIndex = Math.floor(Math.random() * colors.length);
      Module._g_ChangeColor(colors[randomIndex]);
    } else {
      // Sequential mode
      Module._g_ChangeColor(colors[index]);
      index = (index + 1) % colors.length;
    }

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
