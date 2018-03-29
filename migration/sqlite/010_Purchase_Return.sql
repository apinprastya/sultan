CREATE TABLE purchaseitemreturns (
    `id` INTEGER PRIMARY KEY,
    `created_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `updated_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `deleted_at` DATETIME NULL,
    `barcode` VARCHAR(128) NOT NULL DEFAULT '',
    `name` VARCHAR(128) NOT NULL DEFAULT '',
    `count` FLOAT NOT NULL DEFAULT '0',
    `price` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `suplier_id` INT NOT NULL,
    `purchase_item_id` INT NOT NULL,
    `status` TINYINT NOT NULL DEFAULT 0,
    `return_date` DATE NULL,
    `total` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `item_returned` FLOAT NOT NULL DEFAULT 0,
    `money_returned` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `bank_id` INT NOT NULL DEFAULT 0,
    `note` VARCHAR(255) NOT NULL DEFAULT '',
    `user_id` INT NOT NULL
);
-- separator
CREATE INDEX PURCHASEITEMRETURN ON purchaseitemreturns (barcode);