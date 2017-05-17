CREATE TABLE categories (
	`id` INTEGER PRIMARY KEY AUTOINCREMENT,
    `created_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `updated_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `deleted_at` DATETIME NULL,
    `parent_id` INT NOT NULL DEFAULT 0,
    `name` VARCHAR(64) NOT NULL DEFAULT '',
    `code` VARCHAR(16) NOT NULL DEFAULT '',
    `hierarchy` TEXT NULL
);
-- separator
INSERT INTO categories(name, code, hierarchy) 
VALUES ('Food', 'FD', 'Food');
-- separator
INSERT INTO categories(parent_id, name, code, hierarchy)
VALUES
(1, 'Snack', 'SN', 'Food - Snack'),
(1, 'Drink', 'DN', 'Food - Drink');