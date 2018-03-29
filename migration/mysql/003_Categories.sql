CREATE TABLE categories (
	`id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NULL,
    `deleted_at` TIMESTAMP NULL,
    `parent_id` INT NOT NULL DEFAULT 0,
    `name` VARCHAR(64) NOT NULL DEFAULT '',
    `code` VARCHAR(16) NOT NULL DEFAULT '',
    `hierarchy` TEXT NULL,
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

CREATE TABLE categorychilds (
	`id` INT NOT NULL AUTO_INCREMENT,
	`category_id` INT NOT NULL,
	`child_id` INT NOT NULL,
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

INSERT INTO categories(name, code, hierarchy) 
VALUES ('Food', 'FD', 'Food');

INSERT INTO categories(parent_id, name, code, hierarchy)
VALUES
(1, 'Snack', 'SN', 'Food - Snack'),
(1, 'Drink', 'DN', 'Food - Drink');

INSERT INTO categorychilds (category_id, child_id)
VALUES (1, 1), (1, 2), (1, 3), (2, 2), (3, 3);