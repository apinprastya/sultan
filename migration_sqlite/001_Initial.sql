CREATE TABLE users (
	`id` INTEGER PRIMARY KEY,
    `created_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `updated_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `deleted_at` DATETIME NULL,
    `username` TEXT NOT NULL,
    `password` TEXT NOT NULL,
    `name` TEXT NOT NULL DEFAULT '',
    `address` TEXT NOT NULL DEFAULT '',
    `phone` TEXT NOT NULL DEFAULT '',
    `permission` TEXT NOT NULL DEFAULT ''
);

CREATE UNIQUE INDEX USERNAME ON users (username);

INSERT INTO users (username, password, name, address, permission) 
VALUES 
('sultan', '9af82031d374b97c9e73132a413cbdf5', 'Turbin', 'Jogonalan Lor RT 2 Kasihan Bantul Yogyakarta', '9876');
