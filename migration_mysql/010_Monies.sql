-- type : 0 = income, 1 = expense
-- link_type : 0 = transaction, 1 = sold transaction, 2 = purchase, 3 = customer credit payment
CREATE TABLE monies (
    `id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NULL,
    `deleted_at` TIMESTAMP NULL,
    `date` TIMESTAMP NULL,
    `number` VARCHAR(64) NOT NULL,
    `type` TINYINT NOT NULL DEFAULT 0,
    `link_id` INT NOT NULL DEFAULT 0,
    `link_type` TINYINT NOT NULL DEFAULT 0,
    `bank_id` INT NOT NULL,
    `detail` VARCHAR(255) NOT NULL,
    `total` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `user_id` INT NOT NULL,
    `machine_id` INT NOT NULL,
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;
