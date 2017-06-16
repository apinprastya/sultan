-- payment type : 1 = direct, 2 = deadline
-- status : 1 = unpaid, 2 = paid
CREATE TABLE purchases (
	`id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NULL,
    `deleted_at` TIMESTAMP NULL,
    `suplier_id` INT NOT NULL,
    `number` VARCHAR(64) NOT NULL DEFAULT '',
    `payment_type` TINYINT NOT NULL DEFAULT 1,
    `payment_number` VARCHAR(64) NULL,
    `payment_date` DATE NULL,
    `deadline` DATE NULL,
    `discount_formula` VARCHAR(255) NOT NULL DEFAULT '',
    `discount` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `total` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `final` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `status` TINYINT NOT NULL DEFAULT 1,
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

CREATE TABLE purchaseitems (
	`id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NULL,
    `deleted_at` TIMESTAMP NULL,
    `purchase_id` INT NOT NULL,
    `barcode` VARCHAR(64) NOT NULL,
    `count` FLOAT NOT NULL DEFAULT 0,
    `price` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `discount_formula` VARCHAR(255) NOT NULL DEFAULT '',
    `discount` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `total` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `final` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    INDEX `BARCODE` (`barcode` ASC),
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;
