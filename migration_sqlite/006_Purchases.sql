-- payment type : 1 = Cash; 2 = Non Cash
CREATE TABLE purchases (
	`id` INTEGER PRIMARY KEY,
    `created_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `updated_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `deleted_at` DATETIME NULL,
    `suplier_id` INT NOT NULL,
    `number` VARCHAR(64) NOT NULL DEFAULT '',
    `payment_type` TINYINT NOT NULL DEFAULT 1,
    `payment_number` VARCHAR(64) NULL,
    `payment_date` DATE NULL,
    `deadline` DATE NULL,
    `total` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `discount` DECIMAL(15, 2) NOT NULL DEFAULT 0
);
-- separator
CREATE TABLE purchaseitems (
	`id` INTEGER PRIMARY KEY,
    `created_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `updated_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `deleted_at` DATETIME NULL,
    `purchase_id` INT NOT NULL,
    `barcode` VARCHAR(64) NOT NULL,
    `count` FLOAT NOT NULL DEFAULT 0,
    `price` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `total` DECIMAL(15, 2) NOT NULL DEFAULT 0
);
-- separator
CREATE INDEX PURCHASEITEM ON purchaseitems (barcode);