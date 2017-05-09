CREATE TABLE users (
	`id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    `deleted_at` TIMESTAMP NULL,
    `username` VARCHAR(32) NOT NULL,
    `password` VARCHAR(32) NOT NULL,
    `name` VARCHAR(64) NOT NULL DEFAULT '',
    `address` VARCHAR(255) NOT NULL DEFAULT '',
    `phone` VARCHAR(64) NOT NULL DEFAULT '',
    `permission` VARCHAR(255) NOT NULL DEFAULT '',
    UNIQUE INDEX `USERNAME` (`username` ASC),
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

INSERT INTO users (username, password, permission) VALUES ('turbin', '1a2d7c80190ef949268c5cacddfc2a2b', '9876');