CREATE TABLE checkstocks (
    `id` INTEGER PRIMARY KEY,
    `created_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `updated_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `deleted_at` DATETIME NULL,
    `barcode` VARCHAR(128) NOT NULL DEFAULT '',
    `name` VARCHAR(128) NOT NULL DEFAULT '',
    `system_stock` FLOAT NOT NULL DEFAULT '0',
    `real_stock` FLOAT NOT NULL DEFAULT '0',
    `buy_price` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `total_price` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `note` VARCHAR(255) NOT NULL DEFAULT '',
    `user_id` INT NOT NULL
);
-- separator
CREATE INDEX CHECKSTOCKBARCODE ON checkstocks (barcode);