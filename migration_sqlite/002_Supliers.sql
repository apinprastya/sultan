CREATE TABLE supliers (
	`id` INTEGER PRIMARY KEY,
    `created_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `updated_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `deleted_at` DATETIME NULL,
    `name` VARCHAR(64) NOT NULL,
    `code` VARCHAR(16) NOT NULL,
    `address` VARCHAR(255) NOT NULL DEFAULT '',
    `phone` VARCHAR(64) NOT NULL DEFAULT ''
);
-- separator
CREATE UNIQUE INDEX SUPLIERNAME ON supliers (name);
-- separator
INSERT INTO supliers (name, code, address, phone) VALUES ('CV. Sultan Food', 'TF', 'Jogonalan Lor RT 2', '08123456789,085235419949');