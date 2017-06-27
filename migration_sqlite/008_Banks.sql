CREATE TABLE banks (
    `id` INTEGER PRIMARY KEY,
    `created_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `updated_at` DATETIME DEFAULT (datetime('now', 'localtime')),
    `deleted_at` DATETIME NULL,
    `name` VARCHAR(64) NOT NULL,
    `debit_charge_formula` VARCHAR(256) NOT NULL DEFAULT '',
    `credit_charge_formula` VARCHAR(256) NOT NULL DEFAULT ''
);
-- separator
INSERT INTO banks (name, credit_charge_formula)
VALUES
('BCA', '2.5%');