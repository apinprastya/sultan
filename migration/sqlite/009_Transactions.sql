CREATE TABLE transactions (
    `id` INTEGER PRIMARY KEY,
    `created_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `updated_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `deleted_at` DATETIME NULL,
    `date` DATETIME NULL,
    `number` VARCHAR(64) NOT NULL,
    `type` TINYINT NOT NULL DEFAULT 0,
    `link_id` INT NOT NULL DEFAULT 0,
    `link_type` TINYINT NOT NULL DEFAULT 0,
    `bank_id` INT NOT NULL,
    `detail` VARCHAR(255) NOT NULL,
    `transaction_total` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `money_total` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `user_id` INT NOT NULL,
    `machine_id` INT NOT NULL
);
