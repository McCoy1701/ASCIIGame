/**
 * animations.js
 * * This script handles applying click animations to buttons,
 * including buttons that are loaded dynamically via HTMX.
 */

// This function checks if a clicked element is a button and applies the animation.
function handleButtonClick(event) {
  // event.target is the specific element that was clicked.
  // .closest('.btn') finds the nearest ancestor (or the element itself)
  // that has the class "btn". This is useful if your button contains other
  // elements like icons or spans.
  const button = event.target.closest(".btn");

  // If a button was found, apply the animation.
  if (button) {
    // Add the animation class from your animations.css file.
    button.classList.add("btn-press-anim");

    // IMPORTANT: We must remove the class after the animation is done.
    // The animation duration is 0.3s, which is 300 milliseconds.
    // If we don't remove it, the animation won't play on the next click.
    setTimeout(() => {
      button.classList.remove("btn-press-anim");
    }, 300);
  }
}

// --- Event Delegation Setup ---
// We add a single event listener to the entire document body.
// This listener will catch all click events that "bubble up" from any element,
// including elements that are loaded onto the page later by HTMX.
// This is more efficient and reliable than trying to add listeners after every HTMX swap.
document.body.addEventListener("click", handleButtonClick);

console.log("Button animation handler is active.");
