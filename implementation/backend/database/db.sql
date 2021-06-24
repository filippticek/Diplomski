CREATE TABLE users (
	id		    SERIAL PRIMARY KEY,
	first_name 	VARCHAR,
	last_name	VARCHAR,
	username    VARCHAR,
	password    VARCHAR,
	role        VARCHAR
);

CREATE TABLE devices (
    id          SERIAL PRIMARY KEY,
    name        VARCHAR,
    token       VARCHAR
);

CREATE TABLE readings (
    id			   SERIAL PRIMARY KEY,
    user_device_id INTEGER,
    temperature    DECIMAL,
    time           TIMESTAMP
);

CREATE TABLE users_devices (
    id          INTEGER PRIMARY KEY,
    user_id     INTEGER,
    device_id   INTEGER,
    active      BOOLEAN
);

ALTER TABLE readings
ADD CONSTRAINT fk_user)device FOREIGN KEY (user_device_id) REFERENCES users_devices (id);
ALTER TABLE users_devices
ADD CONSTRAINT fk_devices FOREIGN KEY (device_id) REFERENCES devices (id);
ALTER TABLE users_devices
ADD CONSTRAINT fk_users FOREIGN KEY (user_id) REFERENCES users (id);

