CREATE TABLE units (
    `id` INT NOT NULL AUTO_INCREMENT,
    `deleted_at` TIMESTAMP NULL,
    `name` VARCHAR(64) NOT NULL DEFAULT '',
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

INSERT INTO units (name) VALUES ('pcs'), ('box'), ('renteng'), ('kg'), ('gram'), ('liter');

ALTER TABLE items ADD COLUMN `unit` VARCHAR(64) NOT NULL DEFAULT '';
ALTER TABLE items ADD COLUMN `flag` INT NOT NULL DEFAULT 0;
ALTER TABLE items ADD COLUMN `note` VARCHAR(255) NOT NULL DEFAULT '';
ALTER TABLE items ADD COLUMN `barcode_link` VARCHAR(255) NOT NULL DEFAULT '';
ALTER TABLE items ADD COLUMN `count_link` FLOAT NOT NULL DEFAULT '0';
ALTER TABLE items ADD COLUMN `picture` VARCHAR(255) NOT NULL DEFAULT '';
ALTER TABLE solditems ADD COLUMN `unit` VARCHAR(64) NOT NULL DEFAULT '';

ALTER TABLE checkstocks ADD COLUMN `flag` INT NOT NULL DEFAULT 0;

ALTER TABLE purchaseitemreturns ADD COLUMN `machine_id` INT NOT NULL;

UPDATE items SET unit = 'pcs';
UPDATE items set flag = 89;