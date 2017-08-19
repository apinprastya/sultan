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
ALTER TABLE checkstocks ADD COLUMN `flag` INT NOT NULL DEFAULT 0;
-- separator
UPDATE items SET unit = 'pcs';