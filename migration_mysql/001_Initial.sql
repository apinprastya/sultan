CREATE TABLE users (
	`id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NULL,
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

INSERT INTO users (username, password, name, address, permission) 
VALUES 
('sultan', '9af82031d374b97c9e73132a413cbdf5', 'Sultan', 'Jogonalan Lor RT 2 Kasihan Bantul Yogyakarta', '9876');