CREATE TABLE customers (
	`id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NULL,
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
CREATE TABLE customerrewards (
	`id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `deleted_at` TIMESTAMP NULL,
    `customer_id` INT NOT NULL,
    `number` VARCHAR(64) NOT NULL,
    `link_id` INT NOT NULL DEFAULT 0,
    `detail` VARCHAR(255) NOT NULL DEFAULT '',
    `reward` INT NOT NULL DEFAULT 0,
    `total_shop` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `user_id` INT NOT NULL,
    INDEX `CUSTOMER_REWARD_CUSTOMER_ID` (`customer_id` ASC),
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

-- number : solds number when adding credit and dedicated number when it payed
CREATE TABLE customercredits (
	`id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `deleted_at` TIMESTAMP NULL,
    `customer_id` INT NOT NULL,
    `number` VARCHAR(64) NOT NULL,
    `link_id` INT NOT NULL DEFAULT 0,
    `detail` VARCHAR(255) NOT NULL DEFAULT '',
    `credit` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `bank_id` INT NOT NULL DEFAULT 0,
    `user_id` INT NOT NULL,
    `machine_id` INT NOT NULL,
    INDEX `CUSTOMER_CREDIT_CUSTOMER_ID` (`customer_id` ASC),
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

CREATE TABLE rewards (
    `id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NULL,
    `deleted_at` TIMESTAMP NULL,
    `count` INT NOT NULL DEFAULT 0,
    `detail` VARCHAR(255) NOT NULL DEFAULT '',
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

CREATE TABLE rewardpoins (
    `id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NULL,
    `deleted_at` TIMESTAMP NULL,
    `total` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `poin` INT NOT NULL DEFAULT 0,
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

INSERT INTO customers (number, name) VALUES ('123', 'Johan');