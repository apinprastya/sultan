CREATE TABLE stockcards (
    `id` INTEGER PRIMARY KEY,
    `created_at` DATETIME DEFAULT (datetime('now', 'localtime')),
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
    `link_id` INT NOT NULL DEFAULT 0
);
-- separator
CREATE INDEX STOCKCARDBARCODE ON stockcards (barcode);