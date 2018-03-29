CREATE TABLE units (
    `id` INTEGER PRIMARY KEY,
    `deleted_at` TIMESTAMP NULL,
    `name` VARCHAR(64) NOT NULL DEFAULT ''
);
-- separator
INSERT INTO units (name) VALUES ('pcs'), ('box'), ('renteng'), ('kg'), ('gram'), ('liter');
-- separator
ALTER TABLE items ADD COLUMN `unit` VARCHAR(64) NOT NULL DEFAULT '';
-- separator
ALTER TABLE items ADD COLUMN `flag` INT NOT NULL DEFAULT 0;
-- separator
ALTER TABLE items ADD COLUMN `note` VARCHAR(255) NOT NULL DEFAULT '';
-- separator
ALTER TABLE items ADD COLUMN `barcode_link` VARCHAR(255) NOT NULL DEFAULT '';
-- separator
ALTER TABLE items ADD COLUMN `count_link` FLOAT NOT NULL DEFAULT '0';
-- separator
ALTER TABLE items ADD COLUMN `picture` VARCHAR(255) NOT NULL DEFAULT '';
-- separator
ALTER TABLE solditems ADD COLUMN `unit` VARCHAR(64) NOT NULL DEFAULT '';
-- separator
ALTER TABLE checkstocks ADD COLUMN `flag` INT NOT NULL DEFAULT 0;
-- separator
ALTER TABLE purchaseitemreturns ADD COLUMN `machine_id` INT NOT NULL DEFAULT 0;
-- separator
UPDATE items SET unit = 'pcs';
-- separator
UPDATE items set flag = 89;