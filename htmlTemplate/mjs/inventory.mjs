/*
 * Inventory Module for ASCII Game Interface
 * Location: htmlTemplate/mjs/inventory.mjs
 *
 * This module handles all inventory-related functionality including
 * item rendering, interaction, and state management.
 */

import Utils from "./utils.mjs";

// Inventory data structure
let inventoryData = {
  items: [
    {
      id: "iron-sword",
      name: "Iron Sword",
      type: "weapon",
      icon: "fas fa-sword",
      rarity: "common",
      damage: "8-12",
      description: "A sturdy iron blade, well-balanced for combat.",
      quantity: 1,
      equipped: true,
    },
    {
      id: "health-potion",
      name: "Health Potion",
      type: "consumable",
      icon: "fas fa-flask",
      rarity: "common",
      effect: "Restores 50 HP",
      description: "A red liquid that quickly heals wounds.",
      quantity: 3,
      stackable: true,
    },
    {
      id: "ancient-key",
      name: "Ancient Kesssssssy",
      type: "key",
      icon: "fas fa-key",
      rarity: "rare",
      description: "An ornate key with mysterious engravings.",
      quantity: 1,
      quest_item: true,
    },
  ],
  maxSlots: 20,
  categories: ["all", "weapons", "armor", "consumables", "misc"],
};

// Configuration
const config = {
  gridColumns: 3,
  showQuantities: true,
  showTooltips: true,
  enableSearch: false,
  enableSorting: true,
};

// Current state
let currentFilter = "all";
let searchQuery = "";
let sortBy = "name";

// Initialize inventory module
export function initialize() {
  console.log("Inventory module initialize() called");
  Utils.Console.system("Inventory module initializing...");

  // Load saved inventory data
  const savedData = Utils.Storage.get("inventory", null);
  if (savedData) {
    inventoryData = Utils.Data.merge(inventoryData, savedData);
    Utils.Console.info("Loaded inventory from storage");
  }

  // Set up event listeners
  setupEventListeners();

  Utils.Console.success("Inventory module initialized");
  console.log("Inventory module initialized successfully");
  return Promise.resolve(true);
}

// Set up event listeners
function setupEventListeners() {
  // Listen for inventory updates from other modules
  Utils.Events.on("inventory:add", handleAddItem);
  Utils.Events.on("inventory:remove", handleRemoveItem);
  Utils.Events.on("inventory:update", handleUpdateItem);
  Utils.Events.on("inventory:clear", handleClearInventory);

  // Listen for player actions
  Utils.Events.on("player:item-use", handleItemUse);
  Utils.Events.on("player:item-equip", handleItemEquip);
}

// Generate inventory screen HTML
export function generateContent() {
  Utils.Debug.time("inventory:generateContent");

  const filteredItems = getFilteredItems();
  const emptySlots = Math.max(
    0,
    inventoryData.maxSlots - inventoryData.items.length,
  );

  const content = `
        <div class="inventory-header">
            <div class="inventory-info">
                <span class="item-count">${inventoryData.items.length}/${inventoryData.maxSlots} slots</span>
                <span class="weight-info">Weight: ${calculateTotalWeight()} kg</span>
            </div>
            ${config.enableSearch ? generateSearchBar() : ""}
            ${generateFilterTabs()}
        </div>
        <div class="inventory-footer">
            ${generateActionButtons()}
        </div>
        ${generateInventoryGrid(filteredItems, emptySlots)}

    `;

  Utils.Debug.timeEnd("inventory:generateContent");
  return content;
}

// Generate search bar
function generateSearchBar() {
  return `
        <div class="inventory-search">
            <input type="text"
                   placeholder="Search items..."
                   value="${searchQuery}"
                   class="search-input"
                   onkeyup="handleInventorySearch(this.value)"
                   style="padding: 4px 8px; font-size: 12px; border: 1px solid var(--border-secondary); background: var(--bg-tertiary); color: var(--text-primary); width: 100%; box-sizing: border-box;">
        </div>
    `;
}

// Generate filter tabs
function generateFilterTabs() {
  return `
        <div class="inventory-filters" style="display: flex; gap: 4px; margin-top: 8px;">
            ${inventoryData.categories
              .map(
                (category) => `
                <button class="filter-tab ${currentFilter === category ? "active" : ""}"
                        onclick="handleFilterChange('${category}')"
                        style="padding: 4px 8px; font-size: 11px; border: 1px solid var(--border-secondary); background: ${currentFilter === category ? "var(--accent-color)" : "var(--bg-tertiary)"}; color: var(--text-primary); cursor: pointer; border-radius: 2px;">
                    ${category.charAt(0).toUpperCase() + category.slice(1)}
                </button>
            `,
              )
              .join("")}
        </div>
    `;
}

// Generate inventory grid
function generateInventoryGrid(items, emptySlots) {
  const gridItems = items.map((item) => generateInventorySlot(item, true));

  // Add empty slots
  for (let i = 0; i < emptySlots; i++) {
    gridItems.push(generateInventorySlot(null, false));
  }

  return `
        <div class="inventory-grid">
            ${gridItems.join("")}
        </div>
    `;
}

// Generate individual inventory slot
function generateInventorySlot(item, filled) {
  if (!filled) {
    return `<div class="inventory-slot empty"></div>`;
  }

  const equippedClass = item.equipped ? "equipped" : "";
  const questClass = item.quest_item ? "quest-item" : "";

  return `
        <div class="btn inventory-slot filled ${equippedClass} ${questClass}"
             data-item-id="${item.id}"
             onclick="handleItemClick('${item.id}')"
             ${config.showTooltips ? `title="${generateTooltipText(item)}"` : ""}>
            <i class="${item.icon}"></i>
            <span class="inventory-item-name">${item.name}</span>
            ${item.quantity > 1 && config.showQuantities ? `<div class="item-quantity">${item.quantity}</div>` : ""}
            ${item.equipped ? '<div class="equipped-indicator">E</div>' : ""}
            ${item.quest_item ? '<div class="quest-indicator">Q</div>' : ""}
        </div>
    `;
}

// Generate action buttons
function generateActionButtons() {
  return `
        <div class="inventory-actions" style="display: flex; gap: 4px;">
            <button class="action-btn" onclick="handleSortInventory()"
                    style="padding: 4px 8px; font-size: 11px; border: 1px solid var(--border-secondary); background: var(--bg-tertiary); color: var(--text-primary); cursor: pointer; border-radius: 2px; display: flex; align-items: center; gap: 4px;">
                <i class="fas fa-sort"></i> Sort: ${sortBy}
            </button>
            <button class="action-btn" onclick="handleDropAllJunk()"
                    style="padding: 4px 8px; font-size: 11px; border: 1px solid var(--border-secondary); background: var(--bg-tertiary); color: var(--text-primary); cursor: pointer; border-radius: 2px; display: flex; align-items: center; gap: 4px;">
                <i class="fas fa-trash"></i> Drop Junk
            </button>
        </div>
    `;
}

// Generate tooltip text for item
function generateTooltipText(item) {
  let tooltip = `${item.name}\n`;
  tooltip += `Type: ${item.type}\n`;
  if (item.rarity) tooltip += `Rarity: ${item.rarity}\n`;
  if (item.damage) tooltip += `Damage: ${item.damage}\n`;
  if (item.effect) tooltip += `Effect: ${item.effect}\n`;
  if (item.description) tooltip += `\n${item.description}`;
  return tooltip;
}

// Item interaction handlers
export function handleItemClick(itemId) {
  const item = findItemById(itemId);
  if (!item) {
    Utils.Console.warning(`Item not found: ${itemId}`);
    return;
  }

  Utils.Console.info(`Clicked item: ${item.name}`);

  // Emit event for other modules to listen
  Utils.Events.emit("inventory:item-clicked", { item, action: "inspect" });

  // Show item details in object inspector
  displayItemInInspector(item);

  // Log different messages based on item type
  switch (item.type) {
    case "weapon":
      Utils.Console.info(`${item.name} - Damage: ${item.damage || "Unknown"}`);
      break;
    case "consumable":
      Utils.Console.info(`${item.name} - Effect: ${item.effect || "Unknown"}`);
      break;
    case "key":
      Utils.Console.info(`${item.name} - A special key item`);
      break;
    default:
      Utils.Console.info(
        `${item.name} - ${item.description || "No description available"}`,
      );
  }
}

// Right-click context menu handler

// Display item in the object inspector
function displayItemInInspector(item) {
  const inspectorData = {
    name: item.name,
    type: item.type,
    rarity: item.rarity || "common",
    description: item.description || "No description available",
  };

  // Add type-specific properties
  if (item.damage) inspectorData.damage = item.damage;
  if (item.effect) inspectorData.effect = item.effect;
  if (item.quantity > 1) inspectorData.quantity = item.quantity;
  if (item.equipped) inspectorData.status = "Equipped";
  if (item.quest_item) inspectorData.special = "Quest Item";

  // Emit event to update object inspector
  Utils.Events.emit("object-inspector:display", {
    object: inspectorData,
    category: "item",
  });
}

// Show context menu for item

// Get available actions for an item
function getItemActions(item) {
  const actions = [];

  if (item.type === "consumable") {
    actions.push({
      label: "Use",
      handler: (item) => useItem(item.id),
    });
  }

  if (item.type === "weapon" || item.type === "armor") {
    actions.push({
      label: item.equipped ? "Unequip" : "Equip",
      handler: (item) => toggleEquip(item.id),
    });
  }

  if (!item.quest_item) {
    actions.push({
      label: "Drop",
      handler: (item) => dropItem(item.id),
    });
  }

  actions.push({
    label: "Inspect",
    handler: (item) => displayItemInInspector(item),
  });

  return actions;
}

// Item action functions
function useItem(itemId) {
  const item = findItemById(itemId);
  if (!item) return;

  Utils.Console.success(`Used ${item.name}`);

  if (item.stackable && item.quantity > 1) {
    item.quantity--;
  } else {
    removeItem(itemId);
  }

  Utils.Events.emit("inventory:item-used", { item });
  saveInventory();
}

function toggleEquip(itemId) {
  const item = findItemById(itemId);
  if (!item) return;

  item.equipped = !item.equipped;
  Utils.Console.info(
    `${item.equipped ? "Equipped" : "Unequipped"} ${item.name}`,
  );

  Utils.Events.emit("inventory:item-equipped", {
    item,
    equipped: item.equipped,
  });
  saveInventory();
}

function dropItem(itemId) {
  const item = findItemById(itemId);
  if (!item) return;

  Utils.Console.warning(`Dropped ${item.name}`);
  removeItem(itemId);
  Utils.Events.emit("inventory:item-dropped", { item });
}

// Utility functions
function findItemById(itemId) {
  return inventoryData.items.find((item) => item.id === itemId);
}

function getFilteredItems() {
  let items = inventoryData.items;

  // Apply category filter
  if (currentFilter !== "all") {
    items = items.filter((item) => {
      switch (currentFilter) {
        case "weapons":
          return item.type === "weapon";
        case "armor":
          return item.type === "armor";
        case "consumables":
          return item.type === "consumable";
        case "misc":
          return !["weapon", "armor", "consumable"].includes(item.type);
        default:
          return true;
      }
    });
  }

  // Apply search filter
  if (searchQuery) {
    items = items.filter(
      (item) =>
        item.name.toLowerCase().includes(searchQuery.toLowerCase()) ||
        (item.description &&
          item.description.toLowerCase().includes(searchQuery.toLowerCase())),
    );
  }

  // Apply sorting
  items.sort((a, b) => {
    switch (sortBy) {
      case "name":
        return a.name.localeCompare(b.name);
      case "type":
        return a.type.localeCompare(b.type);
      case "rarity":
        const rarityOrder = {
          common: 1,
          uncommon: 2,
          rare: 3,
          epic: 4,
          legendary: 5,
        };
        return (rarityOrder[b.rarity] || 0) - (rarityOrder[a.rarity] || 0);
      default:
        return 0;
    }
  });

  return items;
}

function calculateTotalWeight() {
  return inventoryData.items.reduce((total, item) => {
    const weight = item.weight || 1;
    const quantity = item.quantity || 1;
    return total + weight * quantity;
  }, 0);
}

// Event handlers for external events
function handleAddItem(data) {
  const { item } = data;
  inventoryData.items.push(item);
  Utils.Console.success(`Added ${item.name} to inventory`);
  saveInventory();
}

function handleRemoveItem(data) {
  const { itemId } = data;
  removeItem(itemId);
}

function handleUpdateItem(data) {
  const { itemId, updates } = data;
  const item = findItemById(itemId);
  if (item) {
    Object.assign(item, updates);
    saveInventory();
  }
}

function handleClearInventory() {
  inventoryData.items = [];
  Utils.Console.warning("Inventory cleared");
  saveInventory();
}

function handleItemUse(data) {
  const { itemId } = data;
  useItem(itemId);
}

function handleItemEquip(data) {
  const { itemId } = data;
  toggleEquip(itemId);
}

// Remove item from inventory
function removeItem(itemId) {
  const index = inventoryData.items.findIndex((item) => item.id === itemId);
  if (index > -1) {
    const item = inventoryData.items[index];
    inventoryData.items.splice(index, 1);
    Utils.Console.info(`Removed ${item.name} from inventory`);
    saveInventory();
    return item;
  }
  return null;
}

// Save inventory to storage
function saveInventory() {
  Utils.Storage.set("inventory", inventoryData);
}

// Global functions that will be called from HTML
window.handleItemClick = handleItemClick;
window.handleItemRightClick = (event, itemId) => {
  event.preventDefault();
  event.stopPropagation();
  handleItemRightClick(event, itemId);
  return false;
};
window.handleInventorySearch = (query) => {
  searchQuery = query;
  Utils.Events.emit("inventory:refresh");
};
window.handleFilterChange = (filter) => {
  currentFilter = filter;
  Utils.Events.emit("inventory:refresh");
};
window.handleSortInventory = () => {
  // Cycle through sort options
  const sortOptions = ["name", "type", "rarity"];
  const currentIndex = sortOptions.indexOf(sortBy);
  sortBy = sortOptions[(currentIndex + 1) % sortOptions.length];
  Utils.Console.info(`Sorting by: ${sortBy}`);
  Utils.Events.emit("inventory:refresh");
};
window.handleDropAllJunk = () => {
  const junkItems = inventoryData.items.filter(
    (item) => item.rarity === "junk" || item.type === "junk",
  );
  junkItems.forEach((item) => removeItem(item.id));
  Utils.Console.success(`Dropped ${junkItems.length} junk items`);
};
window.handleAutoEquipBest = () => {
  Utils.Console.info("Auto-equipping best items...");
  // Implementation would go here
};

// Export public API
export default {
  initialize,
  generateContent,
  handleItemClick,
  handleItemRightClick,
  getInventoryData: () => inventoryData,
  addItem: (item) => Utils.Events.emit("inventory:add", { item }),
  removeItem: (itemId) => Utils.Events.emit("inventory:remove", { itemId }),
  useItem: (itemId) => Utils.Events.emit("player:item-use", { itemId }),
  equipItem: (itemId) => Utils.Events.emit("player:item-equip", { itemId }),
};
