CREATE TABLE items (
	`barcode` VARCHAR(64) NOT NULL PRIMARY KEY,
    `created_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `updated_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `deleted_at` DATETIME NULL,
    `name` VARCHAR(255) NOT NULL DEFAULT '',
    `category_id` INT NOT NULL DEFAULT 0,
    `suplier_id` INT NOT NULL DEFAULT 0,
    `buy_price` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `stock` FLOAT NOT NULL DEFAULT 0
);
-- separator
CREATE TABLE sellprices (
	`id` INTEGER PRIMARY KEY,
    `created_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `updated_at` DATETIME DEFAULT (datetime('now', 'localtime')),
	`barcode` VARCHAR(64) NOT NULL,
    `count` FLOAT NOT NULL DEFAULT '1',
    `price` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `discount_formula` VARCHAR(256) NOT NULL DEFAULT '',
    `discount` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `final` DECIMAL(15, 2) NOT NULL DEFAULT 0
);
-- separator
CREATE INDEX SELLPRICE ON sellprices (barcode);
-- separator
INSERT INTO items (barcode, name, category_id, suplier_id, buy_price)
VALUES
('123', 'ROTI ISI COKLAT', 1, 1, 5500),
('234', 'KACANG ATOM', 2, 1, 900),
('345', 'CRACKERS', 2, 1, 4000),
('456', 'AIR MINERAL 300ML', 3, 1, 2200),
('567', 'AIR MINERAL 150ML', 3, 1, 400),
('678', 'ROTI ISI KACANG', 1, 1, 5500);
-- separator
INSERT INTO sellprices (barcode, count, price, final)
VALUES
('123', '1', 6000, 6000),
('123', '6', 5900, 5900),
('234', '1', 1000, 1000),
('234', '6', 950, 950),
('345', '1', 4500, 4500),
('456', '1', 2500, 2500),
('456', '6', 2400, 2400),
('567', '1', 500, 500),
('567', '6', 450, 450),
('678', '1', 6000, 6000),
('678', '6', 5900, 5900);