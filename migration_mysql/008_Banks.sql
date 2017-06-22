CREATE TABLE banks (
    `id` INT NOT NULL AUTO_INCREMENT,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NULL,
    `deleted_at` TIMESTAMP NULL,
    `name` VARCHAR(64) NOT NULL,
    `debit_charge_formula` VARCHAR(256) NOT NULL DEFAULT '',
    `credit_charge_formula` VARCHAR(256) NOT NULL DEFAULT '',
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

INSERT INTO banks (name, credit_charge_formula)
VALUES
('BCA', '2.5%');