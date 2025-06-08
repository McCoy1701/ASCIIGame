export function initModuleLoader(Module) {
  console.log("We are OUT -- OF -- THE -- OBJECT -- INITIALIZATION -- ! ");
  console.log("");

  /* -----

  TEST YOUR CODE HERE TO

 ----- */
  Module.change_color(0xff0000); // Red color
  Module._g_ChangeColor(0x00ff00); // Green color
  var returnValue = Module._g_ChangeColor(0x0000ff); // Blue color
  console.log(`return value: ${returnValue}`);
  // return value: undefined (now we know that void functions return undefined)
}

// This makes it globally available so template.html can use it
window.initModuleLoader = initModuleLoader;
