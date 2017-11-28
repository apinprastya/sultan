ALTER TABLE `customercredits` ADD COLUMN `flag` INT NOT NULL DEFAULT 0;
-- separator
ALTER TABLE `customerrewards` ADD COLUMN `flag` INT NOT NULL DEFAULT 0;
-- separator
ALTER TABLE `customers` ADD COLUMN `flag` INT NOT NULL DEFAULT 0;