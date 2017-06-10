CREATE TABLE users (
	`id` INTEGER PRIMARY KEY,
    `created_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `updated_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `deleted_at` DATETIME NULL,
    `username` VARCHAR(32) NOT NULL,
    `password` VARCHAR(32) NOT NULL,
    `name` VARCHAR(64) NOT NULL DEFAULT '',
    `address` VARCHAR(255) NOT NULL DEFAULT '',
    `phone` VARCHAR(64) NOT NULL DEFAULT '',
    `permission` VARCHAR(255) NOT NULL DEFAULT ''
);
-- separator
CREATE UNIQUE INDEX USERNAME ON users (username);
-- separator
INSERT INTO users (username, password, name, address, permission) 
VALUES 
('sultan', '9af82031d374b97c9e73132a413cbdf5', 'Sultan', 'Jogonalan Lor RT 2 Kasihan Bantul Yogyakarta', '9876');
