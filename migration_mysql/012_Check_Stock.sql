CREATE TABLE checkstocks (
    `id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NULL,
    `deleted_at` TIMESTAMP NULL,
    `barcode` VARCHAR(128) NOT NULL DEFAULT '',
    `name` VARCHAR(128) NOT NULL DEFAULT '',
    `system_stock` FLOAT NOT NULL DEFAULT '0',
    `real_stock` FLOAT NOT NULL DEFAULT '0',
    `buy_price` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `total_price` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `note` VARCHAR(255) NOT NULL DEFAULT '',
    `user_id` INT NOT NULL,
    INDEX `BARCODE` (`barcode` ASC),
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;
