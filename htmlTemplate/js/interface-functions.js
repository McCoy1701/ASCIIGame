/* Interface Functions - Add these to your template.html or separate JS file */

// Global state
let activeScreen = "inventory";
let selectedObject = null;
let inventoryModule = null;

// Console drag-to-resize functionality
let isResizingConsole = false;
let startY = 0;
let startHeight = 0;

function initializeConsoleResize() {
  const consoleHeader = document.querySelector(".console-header");
  const consoleArea = document.querySelector(".console-area");

  if (!consoleHeader || !consoleArea) return;

  consoleHeader.addEventListener("mousedown", startResize);
  document.addEventListener("mousemove", doResize);
  document.addEventListener("mouseup", stopResize);
}

function startResize(e) {
  isResizingConsole = true;
  startY = e.clientY;
  const consoleArea = document.querySelector(".console-area");
  startHeight = parseInt(window.getComputedStyle(consoleArea).height, 10);
  consoleArea.classList.add("resizing");
  document.body.style.cursor = "ns-resize";
  e.preventDefault();
}

function doResize(e) {
  if (!isResizingConsole) return;

  const consoleArea = document.querySelector(".console-area");
  const deltaY = startY - e.clientY;
  const newHeight = Math.min(
    Math.max(startHeight + deltaY, 80),
    window.innerHeight * 0.6,
  );

  consoleArea.style.minHeight = newHeight + "px";
  consoleArea.style.maxHeight = newHeight + "px";
}

function stopResize() {
  if (!isResizingConsole) return;

  isResizingConsole = false;
  const consoleArea = document.querySelector(".console-area");
  consoleArea.classList.remove("resizing");
  document.body.style.cursor = "";
}

// Initialize system when DOM is loaded
document.addEventListener("DOMContentLoaded", function () {
  initializeConsoleResize();
  initializeDynamicContent();
  initializeModules();
  setActiveScreen("inventory"); // Default to inventory
});

// Console functions
function updateConsole(message, type = "info", includeTimestamp = true) {
  const output = document.getElementById("console-output");
  const line = document.createElement("div");
  line.className = `console-line ${type} slide-in`;

  if (includeTimestamp) {
    const timestamp = document.createElement("span");
    timestamp.className = "console-timestamp";
    timestamp.textContent = `[${new Date().toLocaleTimeString()}]`;
    line.appendChild(timestamp);
  }

  const messageSpan = document.createElement("span");
  messageSpan.textContent = message;
  line.appendChild(messageSpan);

  output.appendChild(line);
  output.scrollTop = output.scrollHeight;

  // Auto-remove old messages if too many (keep last 100)
  const lines = output.querySelectorAll(".console-line");
  if (lines.length > 100) {
    lines[0].remove();
  }
}

function clearConsole() {
  const output = document.getElementById("console-output");
  output.innerHTML = '<div class="console-line system">Console cleared</div>';
}

function systemMessage(message) {
  updateConsole(message, "system", false);
}

function logCommand(command) {
  updateConsole(`> ${command}`, "command");
}

// Initialize all modules
function initializeModules() {
  console.log("Starting module initialization...");

  import("../mjs/utils.mjs")
    .then((utilsModuleImport) => {
      console.log("Utils module imported:", utilsModuleImport);
      window.Utils = utilsModuleImport.default;
      console.log("Utils attached to window:", window.Utils);
      return import("../mjs/inventory.mjs");
    })
    .then((inventoryModuleImport) => {
      console.log("Inventory module imported:", inventoryModuleImport);
      inventoryModule = inventoryModuleImport.default;
      console.log("Inventory module assigned:", inventoryModule);

      if (inventoryModule && inventoryModule.initialize) {
        return inventoryModule.initialize();
      } else {
        throw new Error("Inventory module or initialize function not found");
      }
    })
    .then(() => {
      console.log("Inventory module initialized, setting up events...");

      // Set up event listeners for module communication
      if (window.Utils && window.Utils.Events) {
        window.Utils.Events.on("inventory:refresh", () => {
          if (activeScreen === "inventory") {
            loadScreenContent("inventory");
          }
        });

        window.Utils.Events.on("object-inspector:display", (data) => {
          displayObjectInInspector(data.object, data.category);
        });

        console.log("Event listeners set up");
      }

      // Force refresh inventory if it's currently active
      if (activeScreen === "inventory") {
        loadScreenContent("inventory");
      }

      updateConsole("Modules initialized successfully", "success");
      console.log("Module initialization complete");
    })
    .catch((error) => {
      updateConsole(`Failed to initialize modules: ${error.message}`, "error");
      console.error("Module initialization error:", error);
      console.error("Error stack:", error.stack);
    });
}

// Dynamic Content System
function initializeDynamicContent() {
  // Create dynamic content container in right sidebar
  const rightSidebar = document.querySelector(".right-sidebar");
  if (!rightSidebar) return;

  // Remove existing quick-actions if it exists
  const existingQuickActions = rightSidebar.querySelector(".quick-actions");
  if (existingQuickActions) {
    existingQuickActions.remove();
  }

  // Create dynamic content container
  const dynamicContent = document.createElement("div");
  dynamicContent.className = "dynamic-content";
  dynamicContent.id = "dynamic-content";
  rightSidebar.appendChild(dynamicContent);

  // Create selected object info in left sidebar
  createSelectedObjectInfo();
}

function createSelectedObjectInfo() {
  const leftSidebar = document.querySelector(".left-sidebar");
  if (!leftSidebar) return;

  const objectInfo = document.createElement("div");
  objectInfo.className = "selected-object-info";
  objectInfo.innerHTML = `
    <h4>Object Inspector</h4>
    <div class="object-buttons">
      <button class="object-btn" onclick="loadRandomItem()">Random Item</button>
      <button class="object-btn" onclick="loadRandomEntity()">Random Entity</button>
      <button class="object-btn" onclick="loadRandomObject()">Random Object</button>
    </div>
    <div class="object-display" id="object-display">
      <div class="no-object">Click a button to inspect an object</div>
    </div>
  `;

  leftSidebar.appendChild(objectInfo);
}

// Screen Management Functions
function setActiveScreen(screenName) {
  activeScreen = screenName;

  // Update button states
  document.querySelectorAll(".icon-btn").forEach((btn) => {
    btn.classList.remove("active");
  });

  // Map screen names to onclick function names
  const functionMap = {
    inventory: "openInventory",
    "character-sheet": "openCharacterSheet",
    progress: "openProgress",
    map: "openMap",
    quests: "openQuests",
    settings: "openSettings",
  };

  // Find and activate the corresponding button
  const functionName = functionMap[screenName];
  if (functionName) {
    const activeButton = document.querySelector(`[onclick*="${functionName}"]`);
    if (activeButton) {
      activeButton.classList.add("active");
    }
  }

  // Load the screen content
  loadScreenContent(screenName);
}

function loadScreenContent(screenName) {
  const container = document.getElementById("dynamic-content");
  if (!container) return;

  let content = "";

  switch (screenName) {
    case "inventory":
      content = generateInventoryContent();
      break;
    case "character-sheet":
      content = generateCharacterContent();
      break;
    case "progress":
      content = generateProgressContent();
      break;
    case "map":
      content = generateMapContent();
      break;
    case "quests":
      content = generateQuestContent();
      break;
    case "settings":
      content = generateSettingsContent();
      break;
    default:
      content = '<div class="screen-content">Unknown screen</div>';
  }

  container.innerHTML = content;
}

// Screen Content Generators
function generateInventoryContent() {
  // Check user preference for inventory view (default to simple)
  const useAdvancedView =
    localStorage.getItem("inventory-advanced-view") === "true";

  if (useAdvancedView && inventoryModule) {
    return `
      <div class="screen-content inventory-screen">
        <div class="inventory-header-toggle">
          <h4>Inventory</h4>
          <button class="inventory-toggle-btn" onclick="toggleInventoryView()" title="Switch to Simple View">
            <i class="fas fa-cog"></i>
          </button>
        </div>
        ${inventoryModule.generateContent()}
      </div>
    `;
  }

  // Simple inventory view (default) - use module data if available
  const simpleInventoryData = inventoryModule
    ? inventoryModule.getInventoryData()
    : null;

  return `
    <div class="screen-content inventory-screen">
      <div class="inventory-header-toggle">
        <h4>Inventory</h4>
        <button class="inventory-toggle-btn" onclick="toggleInventoryView()" title="Switch to Advanced View">
          <i class="fas fa-cog"></i>
        </button>
      </div>
      <div class="inventory-grid">
        ${generateSimpleInventorySlots(simpleInventoryData)}
      </div>
    </div>
  `;
}

function generateCharacterContent() {
  return `
    <div class="screen-content character-screen">
      <h4>Character Stats</h4>
      <div class="character-stats">
        <div class="stat-row">
          <span class="stat-name">Strength:</span>
          <span class="stat-value">15</span>
        </div>
        <div class="stat-row">
          <span class="stat-name">Dexterity:</span>
          <span class="stat-value">12</span>
        </div>
        <div class="stat-row">
          <span class="stat-name">Intelligence:</span>
          <span class="stat-value">14</span>
        </div>
        <div class="stat-row">
          <span class="stat-name">Vitality:</span>
          <span class="stat-value">13</span>
        </div>
      </div>
      <div class="character-equipment">
        <h5>Equipment</h5>
        <div class="equipment-slot">Weapon: Iron Sword</div>
        <div class="equipment-slot">Armor: Leather Vest</div>
        <div class="equipment-slot">Ring: None</div>
      </div>
    </div>
  `;
}

function generateProgressContent() {
  return `
    <div class="screen-content progress-screen">
      <h4>Skills & Progress</h4>
      <div class="skill-list">
        <div class="skill-item">
          <span class="skill-name">Mining</span>
          <div class="skill-bar">
            <div class="skill-fill" style="width: 35%"></div>
            <span class="skill-level">Level 5</span>
          </div>
        </div>
        <div class="skill-item">
          <span class="skill-name">Woodcutting</span>
          <div class="skill-bar">
            <div class="skill-fill" style="width: 20%"></div>
            <span class="skill-level">Level 3</span>
          </div>
        </div>
        <div class="skill-item">
          <span class="skill-name">Fishing</span>
          <div class="skill-bar">
            <div class="skill-fill" style="width: 60%"></div>
            <span class="skill-level">Level 8</span>
          </div>
        </div>
      </div>
    </div>
  `;
}

function generateMapContent() {
  return `
    <div class="screen-content map-screen">
      <h4>World Map</h4>
      <div class="map-display">
        <div class="map-legend">
          <div class="legend-item">@ = Player</div>
          <div class="legend-item"># = Wall</div>
          <div class="legend-item">. = Floor</div>
        </div>
        <div class="ascii-map">
          ############<br>
          #..........#<br>
          #....@.....#<br>
          #..........#<br>
          ############
        </div>
      </div>
    </div>
  `;
}

function generateQuestContent() {
  return `
    <div class="screen-content quest-screen">
      <h4>Quest Journal</h4>
      <div class="quest-list">
        <div class="quest-item active">
          <h5>Find the Ancient Artifact</h5>
          <p>Search the abandoned mines for the lost artifact.</p>
          <span class="quest-status">In Progress</span>
        </div>
        <div class="quest-item">
          <h5>Gather 10 Iron Ore</h5>
          <p>Mine iron ore from the mountain caves.</p>
          <span class="quest-status">Completed</span>
        </div>
      </div>
    </div>
  `;
}

function generateSettingsContent() {
  return `
    <div class="screen-content settings-screen">
      <h4>Settings</h4>
      <div class="settings-options">
        <div class="setting-item">
          <label>Sound Volume:</label>
          <input type="range" min="0" max="100" value="50">
        </div>
        <div class="setting-item">
          <label>Graphics Quality:</label>
          <select>
            <option>Low</option>
            <option selected>Medium</option>
            <option>High</option>
          </select>
        </div>
        <div class="setting-item">
          <label>Auto-save:</label>
          <input type="checkbox" checked>
        </div>
      </div>
    </div>
  `;
}

// Object Inspection Functions
function loadRandomItem() {
  const items = [
    {
      name: "Enchanted Blade",
      type: "Weapon",
      damage: "15-20",
      rarity: "Rare",
      description: "A sword imbued with magical properties.",
    },
    {
      name: "Health Potion",
      type: "Consumable",
      effect: "Restores 50 HP",
      rarity: "Common",
      description: "A red liquid that heals wounds.",
    },
    {
      name: "Mithril Ring",
      type: "Accessory",
      effect: "+5 Magic Resist",
      rarity: "Epic",
      description: "A ring forged from pure mithril.",
    },
  ];

  const item = items[Math.floor(Math.random() * items.length)];
  displayObject(item, "item");
}

function loadRandomEntity() {
  const entities = [
    {
      name: "Goblin Warrior",
      type: "Enemy",
      health: "45/45",
      level: 5,
      description: "A fierce goblin wielding a rusty sword.",
    },
    {
      name: "Village Merchant",
      type: "NPC",
      status: "Friendly",
      trades: "Yes",
      description: "An old merchant selling various goods.",
    },
    {
      name: "Forest Wolf",
      type: "Beast",
      health: "30/30",
      level: 3,
      description: "A wild wolf prowling the forest.",
    },
  ];

  const entity = entities[Math.floor(Math.random() * entities.length)];
  displayObject(entity, "entity");
}

function loadRandomObject() {
  const objects = [
    {
      name: "Ancient Chest",
      type: "Container",
      status: "Locked",
      contents: "Unknown",
      description: "An old chest with mysterious engravings.",
    },
    {
      name: "Magic Crystal",
      type: "Resource",
      mana: "50 points",
      rarity: "Uncommon",
      description: "A glowing crystal pulsing with energy.",
    },
    {
      name: "Stone Altar",
      type: "Structure",
      function: "Ritual Site",
      age: "Ancient",
      description: "A weathered altar covered in runes.",
    },
  ];

  const object = objects[Math.floor(Math.random() * objects.length)];
  displayObject(object, "object");
}

function displayObject(obj, category) {
  displayObjectInInspector(obj, category);
}

function displayObjectInInspector(obj, category) {
  const display = document.getElementById("object-display");
  if (!display) return;

  selectedObject = obj;

  let content = `
    <div class="object-info">
      <h5>${obj.name}</h5>
      <div class="object-details">
  `;

  // Display properties dynamically
  Object.keys(obj).forEach((key) => {
    if (key !== "name") {
      content += `<div class="detail-row"><span class="detail-label">${key}:</span> <span class="detail-value">${obj[key]}</span></div>`;
    }
  });

  content += `
      </div>
    </div>
  `;

  display.innerHTML = content;
  updateConsole(`Inspecting ${obj.name} (${category})`, "info");
}

// Interface Functions (simplified)
function openCharacterSheet() {
  setActiveScreen("character-sheet");
}
function openInventory() {
  setActiveScreen("inventory");
}
function openProgress() {
  setActiveScreen("progress");
}
function openMap() {
  setActiveScreen("map");
}
function openQuests() {
  setActiveScreen("quests");
}
function openSettings() {
  setActiveScreen("settings");
}

function inspectItem(itemId) {
  updateConsole(`Examining ${itemId}`, "info");

  // If inventory module is loaded, use its item click handler
  if (inventoryModule && inventoryModule.handleItemClick) {
    inventoryModule.handleItemClick(itemId);
  }
}

// Skill Actions (simplified)
function startMining() {
  updateConsole("Started mining...", "info");
  setActiveSkill("mining");
}

function startWoodcutting() {
  updateConsole("Started woodcutting...", "info");
  setActiveSkill("woodcutting");
}

function startFishing() {
  updateConsole("Started fishing...", "info");
  setActiveSkill("fishing");
}

function startCooking() {
  updateConsole("Started cooking...", "info");
  setActiveSkill("cooking");
}

function setActiveSkill(skillName) {
  document.querySelectorAll(".skill-btn").forEach((btn) => {
    btn.classList.remove("active");
  });

  const skillButton = document.querySelector(`.skill-btn.${skillName}`);
  if (skillButton) {
    skillButton.classList.add("active");
  }
}

// Player stat management functions
function simulateStatRecovery() {
  let currentStamina = parseInt(
    document.getElementById("player-stamina").textContent,
  );
  let currentMana = parseInt(
    document.getElementById("player-mana").textContent,
  );

  const recoveryInterval = setInterval(() => {
    if (currentStamina < 100) {
      currentStamina = Math.min(100, currentStamina + 5);
      updatePlayerStat("stamina", currentStamina, 100);
    }

    if (currentMana < 100) {
      currentMana = Math.min(100, currentMana + 3);
      updatePlayerStat("mana", currentMana, 100);
    }

    if (currentStamina >= 100 && currentMana >= 100) {
      clearInterval(recoveryInterval);
      updateConsole("Fully recovered!", "success");
    }
  }, 500);
}

function updatePlayerStat(statType, current, max) {
  const percentage = (current / max) * 100;
  const statElement = document.getElementById(`player-${statType}`);
  const fillElement = document.querySelector(`.${statType}-fill`);
  const textElement = fillElement?.parentElement?.querySelector(".stat-text");

  if (statElement) statElement.textContent = current;
  if (fillElement) fillElement.style.width = percentage + "%";
  if (textElement)
    textElement.innerHTML = `<span id="player-${statType}">${current}</span>/${max}`;
}

function updatePlayerStats(health, stamina, mana, xp = null, level = null) {
  updatePlayerStat("health", health, 100);
  updatePlayerStat("stamina", stamina, 100);
  updatePlayerStat("mana", mana, 100);

  if (xp !== null) {
    const xpToNext = 1000;
    updatePlayerStat("xp", xp, xpToNext);
  }

  if (level !== null) {
    const levelElement = document.getElementById("player-level");
    if (levelElement) levelElement.textContent = level;
  }
}

// Helper function to generate simple inventory slots
function generateSimpleInventorySlots(inventoryData, defaultSlotCount = 24) {
  if (!inventoryData) {
    // Create multiple empty slots with loading indicators when module isn't loaded
    let slots = "";
    for (let i = 0; i < defaultSlotCount; i++) {
      slots += `
        <div class="inventory-slot empty">
          <div class="loading-indicator loading-fade-out">..</div>
        </div>
      `;
    }
    return slots;
  }

  // Generate slots from actual inventory data
  let slots = "";

  // Add filled slots
  inventoryData.items.forEach((item) => {
    const equippedClass = item.equipped ? "equipped" : "";
    const questClass = item.quest_item ? "quest-item" : "";

    slots += `
      <div class="inventory-slot filled ${equippedClass} ${questClass}" onclick="handleItemClick('${item.id}')" style="position: relative;">
        <i class="${item.icon}"></i>
        <span class="inventory-item-name">${item.name}</span>
        ${item.quantity > 1 ? `<div class="item-quantity">${item.quantity}</div>` : ""}
        ${item.equipped ? '<div class="equipped-indicator">E</div>' : ""}
        ${item.quest_item ? '<div class="quest-indicator">Q</div>' : ""}
      </div>
    `;
  });

  // Add empty slots
  const emptySlots = Math.max(
    0,
    inventoryData.maxSlots - inventoryData.items.length,
  );
  for (let i = 0; i < emptySlots; i++) {
    slots += '<div class="inventory-slot empty"></div>';
  }

  return slots;
}

// Toggle between simple and advanced inventory views
function toggleInventoryView() {
  const currentView =
    localStorage.getItem("inventory-advanced-view") === "true";
  const newView = !currentView;

  localStorage.setItem("inventory-advanced-view", newView.toString());

  updateConsole(
    newView ? "Switched to Advanced Inventory" : "Switched to Simple Inventory",
    "info",
  );

  // Refresh the inventory screen if it's currently active
  if (activeScreen === "inventory") {
    loadScreenContent("inventory");
  }
}

// Make toggle function globally available
window.toggleInventoryView = toggleInventoryView;
