/*
 * Base Filter Module for ASCII Game Interface
 * Location: htmlTemplate/mjs/filter.mjs
 *
 * This module provides a reusable filtering, searching, and sorting system
 * that can be used across different game modules (inventory, quests, etc.)
 */

import Utils from "./utils.mjs";

/**
 * Base Filter class - provides generic filtering functionality
 */
export class BaseFilter {
  constructor(config = {}) {
    this.config = {
      categories: ["all"],
      searchFields: ["name"],
      sortOptions: ["name"],
      enableSearch: true,
      enableSorting: true,
      caseSensitive: false,
      ...config,
    };

    // Current state
    this.currentFilter = this.config.categories[0] || "all";
    this.searchQuery = "";
    this.sortBy = this.config.sortOptions[0] || "name";
    this.sortDirection = "asc"; // asc or desc

    // Event emitter for filter changes
    this.eventPrefix = this.config.eventPrefix || "filter";

    this.setupEventHandlers();
  }

  /**
   * Set up event handlers for filter changes
   */
  setupEventHandlers() {
    // Auto-bind methods for HTML event handlers
    this.handleFilterChange = this.handleFilterChange.bind(this);
    this.handleSearch = this.handleSearch.bind(this);
    this.handleSort = this.handleSort.bind(this);
  }

  /**
   * Filter data based on current filter, search, and sort settings
   * @param {Array} data - Array of items to filter
   * @returns {Array} Filtered and sorted data
   */
  filterData(data) {
    if (!Array.isArray(data)) {
      Utils.Console.warning("Filter received non-array data");
      return [];
    }

    let filteredData = [...data];

    // Apply category filter
    filteredData = this.applyCategoryFilter(filteredData);

    // Apply search filter
    filteredData = this.applySearchFilter(filteredData);

    // Apply sorting
    filteredData = this.applySorting(filteredData);

    return filteredData;
  }

  /**
   * Apply category filtering - can be overridden by subclasses
   * @param {Array} data - Data to filter
   * @returns {Array} Filtered data
   */
  applyCategoryFilter(data) {
    if (this.currentFilter === "all") {
      return data;
    }

    // Use custom filter function if provided
    const filterFunction = this.config.categoryFilters?.[this.currentFilter];
    if (filterFunction && typeof filterFunction === "function") {
      return data.filter(filterFunction);
    }

    // Default filtering by type property
    return data.filter((item) => item.type === this.currentFilter);
  }

  /**
   * Apply search filtering
   * @param {Array} data - Data to filter
   * @returns {Array} Filtered data
   */
  applySearchFilter(data) {
    if (!this.searchQuery.trim()) {
      return data;
    }

    const query = this.config.caseSensitive
      ? this.searchQuery
      : this.searchQuery.toLowerCase();

    return data.filter((item) => {
      return this.config.searchFields.some((field) => {
        const fieldValue = this.getNestedProperty(item, field);
        if (fieldValue == null) return false;

        const stringValue = this.config.caseSensitive
          ? String(fieldValue)
          : String(fieldValue).toLowerCase();

        return stringValue.includes(query);
      });
    });
  }

  /**
   * Apply sorting
   * @param {Array} data - Data to sort
   * @returns {Array} Sorted data
   */
  applySorting(data) {
    if (!this.sortBy) {
      return data;
    }

    // Use custom sort function if provided
    const sortFunction = this.config.sortFunctions?.[this.sortBy];
    if (sortFunction && typeof sortFunction === "function") {
      return data.sort((a, b) => {
        const result = sortFunction(a, b);
        return this.sortDirection === "desc" ? -result : result;
      });
    }

    // Default sorting
    return data.sort((a, b) => {
      const aValue = this.getNestedProperty(a, this.sortBy);
      const bValue = this.getNestedProperty(b, this.sortBy);

      let result = 0;
      if (aValue < bValue) result = -1;
      else if (aValue > bValue) result = 1;

      return this.sortDirection === "desc" ? -result : result;
    });
  }

  /**
   * Get nested property value (e.g., "user.profile.name")
   * @param {Object} obj - Object to get property from
   * @param {string} path - Property path
   * @returns {any} Property value
   */
  getNestedProperty(obj, path) {
    return path
      .split(".")
      .reduce(
        (current, prop) =>
          current && current[prop] !== undefined ? current[prop] : null,
        obj,
      );
  }

  /**
   * Generate HTML for filter tabs
   * @returns {string} HTML string
   */
  generateFilterTabs() {
    if (this.config.categories.length <= 1) {
      return "";
    }

    return `
      <div class="filter-tabs" style="display: flex; gap: 4px; margin-top: 8px;">
        ${this.config.categories
          .map(
            (category) => `
          <button class="filter-tab ${this.currentFilter === category ? "active" : ""}"
                  onclick="${this.eventPrefix}_handleFilterChange('${category}')"
                  style="padding: 4px 8px; font-size: 11px; border: 1px solid var(--border-secondary);
                         background: ${this.currentFilter === category ? "var(--accent-color)" : "var(--bg-tertiary)"};
                         color: var(--text-primary); cursor: pointer; border-radius: 2px;">
            ${this.formatCategoryName(category)}
          </button>
        `,
          )
          .join("")}
      </div>
    `;
  }

  /**
   * Generate HTML for search bar
   * @returns {string} HTML string
   */
  generateSearchBar() {
    if (!this.config.enableSearch) {
      return "";
    }

    return `
      <div class="filter-search">
        <input type="text"
               placeholder="${this.config.searchPlaceholder || "Search..."}"
               value="${this.searchQuery}"
               class="search-input"
               onkeyup="${this.eventPrefix}_handleSearch(this.value)"
               style="padding: 4px 8px; font-size: 12px; border: 1px solid var(--border-secondary);
                      background: var(--bg-tertiary); color: var(--text-primary); width: 100%; box-sizing: border-box;">
      </div>
    `;
  }

  /**
   * Generate HTML for sort controls
   * @returns {string} HTML string
   */
  generateSortControls() {
    if (!this.config.enableSorting || this.config.sortOptions.length <= 1) {
      return "";
    }

    return `
      <div class="sort-controls" style="display: flex; gap: 4px; align-items: center;">
        <button class="sort-btn" onclick="${this.eventPrefix}_handleSort()"
                style="padding: 4px 8px; font-size: 11px; border: 1px solid var(--border-secondary);
                       background: var(--bg-tertiary); color: var(--text-primary); cursor: pointer; border-radius: 2px;
                       display: flex; align-items: center; gap: 4px;">
          <i class="fas fa-sort${this.sortDirection === "desc" ? "-down" : "-up"}"></i>
          Sort: ${this.formatSortName(this.sortBy)}
        </button>
      </div>
    `;
  }

  /**
   * Handle filter change
   * @param {string} newFilter - New filter category
   */
  handleFilterChange(newFilter) {
    if (this.currentFilter === newFilter) return;

    this.currentFilter = newFilter;
    Utils.Console.info(`Filter changed to: ${newFilter}`);
    this.emitFilterChange();
  }

  /**
   * Handle search input
   * @param {string} query - Search query
   */
  handleSearch(query) {
    this.searchQuery = query || "";
    Utils.Console.debug(`Search query: "${this.searchQuery}"`);
    this.emitFilterChange();
  }

  /**
   * Handle sort change - cycles through sort options
   */
  handleSort() {
    const currentIndex = this.config.sortOptions.indexOf(this.sortBy);
    const nextIndex = (currentIndex + 1) % this.config.sortOptions.length;

    // If we've cycled through all options, toggle direction
    if (nextIndex === 0 && this.config.sortOptions.length > 1) {
      this.sortDirection = this.sortDirection === "asc" ? "desc" : "asc";
    }

    this.sortBy = this.config.sortOptions[nextIndex];
    Utils.Console.info(`Sorting by: ${this.sortBy} (${this.sortDirection})`);
    this.emitFilterChange();
  }

  /**
   * Emit filter change event
   */
  emitFilterChange() {
    Utils.Events.emit(`${this.eventPrefix}:changed`, {
      filter: this.currentFilter,
      search: this.searchQuery,
      sortBy: this.sortBy,
      sortDirection: this.sortDirection,
    });
  }

  /**
   * Format category name for display
   * @param {string} category - Category name
   * @returns {string} Formatted name
   */
  formatCategoryName(category) {
    return category.charAt(0).toUpperCase() + category.slice(1);
  }

  /**
   * Format sort name for display
   * @param {string} sortBy - Sort field name
   * @returns {string} Formatted name
   */
  formatSortName(sortBy) {
    return sortBy.charAt(0).toUpperCase() + sortBy.slice(1);
  }

  /**
   * Reset all filters to default
   */
  reset() {
    this.currentFilter = this.config.categories[0] || "all";
    this.searchQuery = "";
    this.sortBy = this.config.sortOptions[0] || "name";
    this.sortDirection = "asc";
    this.emitFilterChange();
  }

  /**
   * Get current filter state
   * @returns {Object} Current state
   */
  getState() {
    return {
      filter: this.currentFilter,
      search: this.searchQuery,
      sortBy: this.sortBy,
      sortDirection: this.sortDirection,
    };
  }

  /**
   * Set filter state
   * @param {Object} state - State to set
   */
  setState(state) {
    if (state.filter !== undefined) this.currentFilter = state.filter;
    if (state.search !== undefined) this.searchQuery = state.search;
    if (state.sortBy !== undefined) this.sortBy = state.sortBy;
    if (state.sortDirection !== undefined)
      this.sortDirection = state.sortDirection;
    this.emitFilterChange();
  }
}

/**
 * Inventory-specific filter class
 */
export class InventoryFilter extends BaseFilter {
  constructor(config = {}) {
    const inventoryConfig = {
      categories: ["all", "weapons", "armor", "consumables", "misc"],
      searchFields: ["name", "description"],
      sortOptions: ["name", "type", "rarity"],
      enableSearch: true,
      enableSorting: true,
      searchPlaceholder: "Search items...",
      eventPrefix: "inventory",

      // Custom filter functions for inventory categories
      categoryFilters: {
        weapons: (item) => item.type === "weapon",
        armor: (item) => item.type === "armor",
        consumables: (item) => item.type === "consumable",
        misc: (item) => !["weapon", "armor", "consumable"].includes(item.type),
      },

      // Custom sort functions for inventory
      sortFunctions: {
        rarity: (a, b) => {
          const rarityOrder = {
            common: 1,
            uncommon: 2,
            rare: 3,
            epic: 4,
            legendary: 5,
          };
          const aRarity = rarityOrder[a.rarity] || 0;
          const bRarity = rarityOrder[b.rarity] || 0;
          return bRarity - aRarity; // Higher rarity first
        },
      },
      ...config,
    };

    super(inventoryConfig);
  }
}

export default BaseFilter;
