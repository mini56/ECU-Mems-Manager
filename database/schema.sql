CREATE TABLE ecu (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    family TEXT NOT NULL,
    version TEXT NOT NULL,
    manufacturer TEXT,
    hardware_reference TEXT,
    software_reference TEXT,
    firmware_version TEXT,
    protocol TEXT,
    connector TEXT,
    notes TEXT
);

CREATE TABLE vehicle (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    make TEXT,
    model TEXT,
    engine TEXT,
    displacement INTEGER,
    year_from INTEGER,
    year_to INTEGER,
    catalyst INTEGER,
    ecu_id INTEGER
);

CREATE TABLE connector (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    ecu_id INTEGER,
    pin INTEGER,
    signal TEXT,
    description TEXT
);

CREATE TABLE pid (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    frame TEXT,
    offset INTEGER,
    name TEXT,
    description TEXT,
    unit TEXT,
    formula TEXT,
    minimum REAL,
    maximum REAL
);

CREATE TABLE dtc (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    code TEXT,
    byte INTEGER,
    bit INTEGER,
    label TEXT,
    description TEXT,
    causes TEXT,
    solution TEXT
);

CREATE TABLE actuator (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT,
    command TEXT,
    description TEXT
);

CREATE TABLE adaptation (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT,
    description TEXT
);

CREATE TABLE rom (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    ecu_id INTEGER,
    checksum TEXT,
    size INTEGER,
    filename TEXT
);

CREATE TABLE calibration (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    rom_id INTEGER,
    name TEXT,
    address INTEGER,
    size INTEGER
);