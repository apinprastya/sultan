ALTER TABLE `machines` ADD COLUMN `code` VARCHAR(255) NOT NULL DEFAULT '';
-- separator
CREATE UNIQUE INDEX `CODEMACHINE` ON `machines`(`code`);
-- separator
UPDATE `machines` SET code = name;