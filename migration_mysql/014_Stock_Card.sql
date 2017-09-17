-- type will be : 0: initial stock, 1: purchase, 2 : sold, 3 : checkstocks, 4 : purchase return, 5 : sold return
CREATE TABLE stockcards (
    `id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `barcode` VARCHAR(128) NOT NULL DEFAULT '',
    `name` VARCHAR(128) NOT NULL DEFAULT '',
    `type` INT NOT NULL DEFAULT 0,
    `number` VARCHAR(128) NOT NULL DEFAULT '',
    `count` FLOAT NOT NULL DEFAULT '0',
    `flag` INT NOT NULL DEFAULT 0,
    `count_used` FLOAT NOT NULL DEFAULT 0,
    `count_left` FLOAT NOT NULL DEFAULT 0,
    `price` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `stock` FLOAT NOT NULL DEFAULT 0,
    `link_id` INT NOT NULL DEFAULT 0,
    INDEX `BARCODE` (`barcode` ASC),
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;
