-- type will be : 0: box, 1 : ingridient
CREATE TABLE itemlinks (
    `id` INT NOT NULL AUTO_INCREMENT,
    `type` INT NOT NULL DEFAULT 0,
    `barcode` VARCHAR(128) NOT NULL DEFAULT '',
    `barcode_link` VARCHAR(128) NOT NULL DEFAULT '',
    `count_link` FLOAT NOT NULL DEFAULT '0',
    INDEX `ITEMLINKBARCODE` (`barcode` ASC),
    INDEX `ITEMLINKBARCODELINK` (`barcode_link` ASC),
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

ALTER TABLE solditems MODIFY COLUMN `flag` INT NOT NULL DEFAULT 0;
ALTER TABLE items DROP COLUMN `barcode_link`;
ALTER TABLE items DROP COLUMN `count_link`;
