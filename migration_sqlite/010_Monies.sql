CREATE TABLE monies (
    `id` INTEGER PRIMARY KEY,
    `created_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `updated_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `deleted_at` DATETIME NULL,
    `number` VARCHAR(64) NOT NULL,
    `type` TINYINT NOT NULL DEFAULT 0,
    `bank_id` INT NOT NULL,
    `detail` VARCHAR(255) NOT NULL,
    `total` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `user_id` INT NOT NULL,
    `machine_id` INT NOT NULL
);
