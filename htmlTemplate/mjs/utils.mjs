/*
 * Utility Functions for ASCII Game Interface Modules
 * Location: htmlTemplate/mjs/utils.mjs
 *
 * This module provides shared functionality for all interface modules
 * including console logging, DOM manipulation, and common patterns.
 */

// Console utility functions
export const Console = {
  log: (message, type = "info", includeTimestamp = true) => {
    if (typeof updateConsole === "function") {
      updateConsole(message, type, includeTimestamp);
    } else {
      console.log(`[${type.toUpperCase()}] ${message}`);
    }
  },

  info: (message) => Console.log(message, "info"),
  success: (message) => Console.log(message, "success"),
  warning: (message) => Console.log(message, "warning"),
  error: (message) => Console.log(message, "error"),
  system: (message) => Console.log(message, "system", false),
  command: (message) => Console.log(message, "command"),
};

// DOM utility functions
export const DOM = {
  // Get element with error checking
  get: (selector, parent = document) => {
    const element = parent.querySelector(selector);
    if (!element) {
      Console.warning(`Element not found: ${selector}`);
    }
    return element;
  },

  // Get all elements with error checking
  getAll: (selector, parent = document) => {
    const elements = parent.querySelectorAll(selector);
    if (elements.length === 0) {
      Console.warning(`No elements found: ${selector}`);
    }
    return elements;
  },

  // Create element with attributes and content
  create: (tag, attributes = {}, content = "") => {
    const element = document.createElement(tag);

    Object.entries(attributes).forEach(([key, value]) => {
      if (key === "className" || key === "class") {
        element.className = value;
      } else if (key === "innerHTML") {
        element.innerHTML = value;
      } else if (key === "textContent") {
        element.textContent = value;
      } else if (key.startsWith("data-")) {
        element.setAttribute(key, value);
      } else {
        element[key] = value;
      }
    });

    if (content && !attributes.innerHTML && !attributes.textContent) {
      element.textContent = content;
    }

    return element;
  },

  // Clear container and add new content
  replace: (container, newContent) => {
    if (!container) return false;

    if (typeof newContent === "string") {
      container.innerHTML = newContent;
    } else if (newContent instanceof HTMLElement) {
      container.innerHTML = "";
      container.appendChild(newContent);
    } else if (Array.isArray(newContent)) {
      container.innerHTML = "";
      newContent.forEach((element) => {
        if (element instanceof HTMLElement) {
          container.appendChild(element);
        }
      });
    }
    return true;
  },

  // Add event listener with cleanup tracking
  on: (element, event, handler, options = {}) => {
    if (!element) return null;

    element.addEventListener(event, handler, options);

    // Return cleanup function
    return () => element.removeEventListener(event, handler, options);
  },
};

// Data management utilities
export const Data = {
  // Generate unique IDs
  generateId: (prefix = "item") => {
    return `${prefix}-${Date.now()}-${Math.random().toString(36).substr(2, 9)}`;
  },

  // Validate object structure
  validate: (obj, schema) => {
    const errors = [];

    Object.entries(schema).forEach(([key, requirements]) => {
      if (requirements.required && !(key in obj)) {
        errors.push(`Missing required field: ${key}`);
      }

      if (key in obj && requirements.type) {
        const actualType = typeof obj[key];
        if (actualType !== requirements.type) {
          errors.push(
            `Field ${key} should be ${requirements.type}, got ${actualType}`,
          );
        }
      }
    });

    return {
      valid: errors.length === 0,
      errors,
    };
  },

  // Deep clone object
  clone: (obj) => {
    try {
      return JSON.parse(JSON.stringify(obj));
    } catch (error) {
      Console.error(`Failed to clone object: ${error.message}`);
      return null;
    }
  },

  // Merge objects
  merge: (target, ...sources) => {
    return Object.assign({}, target, ...sources);
  },
};

// Event system for inter-module communication
export const Events = {
  listeners: new Map(),

  // Subscribe to event
  on: (eventName, callback) => {
    if (!Events.listeners.has(eventName)) {
      Events.listeners.set(eventName, []);
    }
    Events.listeners.get(eventName).push(callback);

    // Return unsubscribe function
    return () => Events.off(eventName, callback);
  },

  // Unsubscribe from event
  off: (eventName, callback) => {
    const callbacks = Events.listeners.get(eventName);
    if (callbacks) {
      const index = callbacks.indexOf(callback);
      if (index > -1) {
        callbacks.splice(index, 1);
      }
    }
  },

  // Emit event
  emit: (eventName, data = null) => {
    const callbacks = Events.listeners.get(eventName);
    if (callbacks) {
      callbacks.forEach((callback) => {
        try {
          callback(data);
        } catch (error) {
          Console.error(
            `Error in event callback for ${eventName}: ${error.message}`,
          );
        }
      });
    }
  },

  // Clear all listeners
  clear: () => {
    Events.listeners.clear();
  },
};

// Animation utilities
export const Animation = {
  // Fade in element
  fadeIn: (element, duration = 300) => {
    if (!element) return Promise.resolve();

    return new Promise((resolve) => {
      element.style.opacity = "0";
      element.style.transition = `opacity ${duration}ms ease-in-out`;

      requestAnimationFrame(() => {
        element.style.opacity = "1";
        setTimeout(resolve, duration);
      });
    });
  },

  // Fade out element
  fadeOut: (element, duration = 300) => {
    if (!element) return Promise.resolve();

    return new Promise((resolve) => {
      element.style.transition = `opacity ${duration}ms ease-in-out`;
      element.style.opacity = "0";
      setTimeout(resolve, duration);
    });
  },

  // Slide in from direction
  slideIn: (element, direction = "top", duration = 300) => {
    if (!element) return Promise.resolve();

    const directions = {
      top: "translateY(-20px)",
      bottom: "translateY(20px)",
      left: "translateX(-20px)",
      right: "translateX(20px)",
    };

    return new Promise((resolve) => {
      element.style.transform = directions[direction] || directions.top;
      element.style.opacity = "0";
      element.style.transition = `transform ${duration}ms ease-out, opacity ${duration}ms ease-out`;

      requestAnimationFrame(() => {
        element.style.transform = "translate(0, 0)";
        element.style.opacity = "1";
        setTimeout(resolve, duration);
      });
    });
  },
};

// Storage utilities
export const Storage = {
  // Get from localStorage with JSON parsing
  get: (key, defaultValue = null) => {
    try {
      const item = localStorage.getItem(key);
      return item ? JSON.parse(item) : defaultValue;
    } catch (error) {
      Console.warning(`Failed to get storage item ${key}: ${error.message}`);
      return defaultValue;
    }
  },

  // Set to localStorage with JSON stringification
  set: (key, value) => {
    try {
      localStorage.setItem(key, JSON.stringify(value));
      return true;
    } catch (error) {
      Console.warning(`Failed to set storage item ${key}: ${error.message}`);
      return false;
    }
  },

  // Remove from localStorage
  remove: (key) => {
    try {
      localStorage.removeItem(key);
      return true;
    } catch (error) {
      Console.warning(`Failed to remove storage item ${key}: ${error.message}`);
      return false;
    }
  },

  // Clear all localStorage
  clear: () => {
    try {
      localStorage.clear();
      return true;
    } catch (error) {
      Console.warning(`Failed to clear storage: ${error.message}`);
      return false;
    }
  },
};

// Common patterns for interface modules
export const Interface = {
  // Create a standard screen container
  createScreen: (screenId, title, content = "") => {
    return DOM.create("div", {
      className: `screen-content ${screenId}-screen`,
      innerHTML: `
                <h4>${title}</h4>
                <div class="screen-body">
                    ${content}
                </div>
            `,
    });
  },

  // Create a clickable item element
  createClickableItem: (item, onClick) => {
    const element = DOM.create("div", {
      className: `clickable-item ${item.type || "default"}`,
      "data-item-id": item.id || Data.generateId("item"),
      innerHTML: `
                <div class="item-icon">
                    <i class="${item.icon || "fas fa-cube"}"></i>
                </div>
                <div class="item-info">
                    <span class="item-name">${item.name || "Unknown Item"}</span>
                    ${item.description ? `<span class="item-description">${item.description}</span>` : ""}
                </div>
            `,
    });

    if (onClick) {
      DOM.on(element, "click", () => onClick(item, element));
    }

    return element;
  },

  // Create a grid container
  createGrid: (items, columns = 3, itemRenderer = null) => {
    const grid = DOM.create("div", {
      className: "interface-grid",
      style: `grid-template-columns: repeat(${columns}, 1fr);`,
    });

    items.forEach((item) => {
      const element = itemRenderer
        ? itemRenderer(item)
        : Interface.createClickableItem(item);
      grid.appendChild(element);
    });

    return grid;
  },

  // Standard loading indicator
  createLoader: (text = "Loading...") => {
    return DOM.create("div", {
      className: "interface-loader",
      innerHTML: `
                <div class="loader-spinner"></div>
                <span class="loader-text">${text}</span>
            `,
    });
  },
};

// Error handling utilities
export const ErrorHandler = {
  // Wrap function with error handling
  wrap: (fn, context = "Unknown") => {
    return (...args) => {
      try {
        return fn(...args);
      } catch (error) {
        Console.error(`Error in ${context}: ${error.message}`);
        throw error;
      }
    };
  },

  // Handle async errors
  async: (promise, context = "Unknown") => {
    try {
      return promise;
    } catch (error) {
      Console.error(`Async error in ${context}: ${error.message}`);
      throw error;
    }
  },
};

// Debug utilities
export const Debug = {
  enabled: false,

  log: (...args) => {
    if (Debug.enabled) {
      console.log("[DEBUG]", ...args);
    }
  },

  time: (label) => {
    if (Debug.enabled) {
      console.time(`[DEBUG] ${label}`);
    }
  },

  timeEnd: (label) => {
    if (Debug.enabled) {
      console.timeEnd(`[DEBUG] ${label}`);
    }
  },

  enable: () => {
    Debug.enabled = true;
    Console.system("Debug mode enabled");
  },

  disable: () => {
    Debug.enabled = false;
    Console.system("Debug mode disabled");
  },
};

// Export a default object with all utilities
export default {
  Console,
  DOM,
  Data,
  Events,
  Animation,
  Storage,
  Interface,
  ErrorHandler,
  Debug,
};
