CREATE TABLE itemlinks (
    `id` INTEGER PRIMARY KEY,
    `type` INT NOT NULL DEFAULT 0,
    `barcode` VARCHAR(128) NOT NULL DEFAULT '',
    `barcode_link` VARCHAR(128) NOT NULL DEFAULT '',
    `count_link` FLOAT NOT NULL DEFAULT '0'
);
-- separator
CREATE INDEX ITEMLINKBARCODE ON itemlinks (barcode);
-- separator
CREATE INDEX ITEMLINKBARCODELINK ON itemlinks (barcode_link);
-- separator
BEGIN TRANSACTION;
-- separator
CREATE TABLE t1_backup (
	`barcode` VARCHAR(64) NOT NULL PRIMARY KEY,
    `created_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `updated_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `deleted_at` DATETIME NULL,
    `name` VARCHAR(255) NOT NULL DEFAULT '',
    `category_id` INT NOT NULL DEFAULT 0,
    `suplier_id` INT NOT NULL DEFAULT 0,
    `buy_price` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `stock` FLOAT NOT NULL DEFAULT 0,
    `unit` VARCHAR(64) NOT NULL DEFAULT '',
    `flag` INT NOT NULL DEFAULT 0,
    `note` VARCHAR(255) NOT NULL DEFAULT '',
    `picture` VARCHAR(255) NOT NULL DEFAULT ''
);
-- separator
INSERT INTO t1_backup SELECT barcode,created_at,updated_at,deleted_at,name,
category_id,suplier_id,buy_price,stock,unit,flag,note,picture FROM items;
-- separator
DROP TABLE items;
-- separator
ALTER TABLE t1_backup RENAME TO items;
-- separator
COMMIT;
-- separator
BEGIN TRANSACTION;
-- separator
CREATE TABLE t1_backup (
	`id` INTEGER PRIMARY KEY,
    `created_at` DATETIME DEFAULT (datetime('now', 'localtime')),
	`barcode` VARCHAR(64) NOT NULL,
	`name` VARCHAR(255) NOT NULL DEFAULT '',
	`sold_id` INT NOT NULL,
	`count` FLOAT NOT NULL,
	`price` DECIMAL(15, 2) NOT NULL,
	`total` DECIMAL(15, 2) NOT NULL,
    `discount_formula` VARCHAR(255) NOT NULL DEFAULT '',
    `discount` DECIMAL(15, 2) NOT NULL DEFAULT 0,
    `final` DECIMAL(15, 2) NOT NULL,
    `buy_price` DECIMAL(15, 2) NOT NULL,
    `flag` INT NOT NULL DEFAULT 0,
    `link_id` INT NOT NULL DEFAULT 0,
    `unit` VARCHAR(64) NOT NULL DEFAULT ''
);
-- separator
INSERT INTO t1_backup SELECT id,created_at,barcode,name,sold_id,
count,price,total,discount_formula,discount,final,buy_price,flag,link_id,unit FROM solditems;
-- separator
DROP TABLE solditems;
-- separator
ALTER TABLE t1_backup RENAME TO solditems;
-- separator
COMMIT;