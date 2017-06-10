CREATE TABLE customers (
	`id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    `deleted_at` TIMESTAMP NULL,
    `number` VARCHAR(64) NOT NULL,
    `name` VARCHAR(64) NOT NULL DEFAULT '',
    `address` VARCHAR(255) NOT NULL DEFAULT '',
    `phone` VARCHAR(64) NOT NULL DEFAULT '',
    `reward` INT NOT NULL DEFAULT 0,
    `credit` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    UNIQUE INDEX `NUMBER` (`number` ASC),
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

-- link id : value is the sold id, when customer shop
CREATE TABLE customer_points (
	`id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    `deleted_at` TIMESTAMP NULL,
    `customer_id` INT NOT NULL,
    `number` VARCHAR(64) NOT NULL,
    `link_id` INT NOT NULL DEFAULT 0,
    `detail` VARCHAR(255) NOT NULL DEFAULT '',
    `reward` INT NOT NULL DEFAULT 0,
    `total_shop` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    INDEX `CUSTOMER_POINT_CUSTOMER_ID` (`customer_id` ASC),
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

--number : solds number when adding credit and dedicated number when it payed
CREATE TABLE customer_credits (
	`id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    `deleted_at` TIMESTAMP NULL,
    `customer_id` INT NOT NULL,
    `number` VARCHAR(64) NOT NULL,
    `link_id` INT NOT NULL DEFAULT 0,
    `detail` VARCHAR(255) NOT NULL DEFAULT '',
    `credit` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    INDEX `CUSTOMER_DEBT_CUSTOMER_ID` (`customer_id` ASC),
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

ALTER TABLE solds ADD COLUMN customer_id INT NOT NULL DEFAULT 0;