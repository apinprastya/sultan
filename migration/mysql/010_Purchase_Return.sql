-- return_type : 0 = item, 1 = money
-- status : 0 = unreturn, 1 = returned
-- price will be final price (price - discount)
CREATE TABLE purchaseitemreturns (
    `id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NULL,
    `deleted_at` TIMESTAMP NULL,
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
    `user_id` INT NOT NULL,
    INDEX `BARCODE` (`barcode` ASC),
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;
