package main

import (
	"strconv"
	"time"

	"github.com/gofiber/fiber/v2"
)

func GetUsers(c *fiber.Ctx) error {
	users := &GetUsersResponse{}
	err := DB.Table("users").Scan(&users.Users).Error
	if err != nil {
		return c.SendStatus(404)
	}
	if users == nil {
		return c.SendStatus(404)
	}
	return c.JSON(&users)
}

func GetUser(c *fiber.Ctx) error {
	id, _ := strconv.Atoi(c.Params("id"))
	user := &GetUserResponse{}
	err := DB.Table("users").Where("id = ?", id).Scan(&user).Error
	if err != nil {
		return err
	}
	return c.JSON(&user)
}

func CreateUser(c *fiber.Ctx) error {
	user := &CreateUserRequest{}
	if err := c.BodyParser(&user); err != nil {
		return err
	}
	user.Role = "PATIENT"
	err := DB.Table("users").Create(&user).Error
	if err != nil {
		return err
	}

	return c.JSON(&CreateUserResponse{
		Status: "ok",
	})
}

func UpdateUser(c *fiber.Ctx) error {
	id, _ := strconv.Atoi(c.Params("id"))
	user := &UpdateUserRequest{}
	if err := c.BodyParser(&user); err != nil {
		return err
	}
	err := DB.Table("users").Where("id = ?", id).Updates(&user).Error
	if err != nil {
		return err
	}

	return c.JSON(&UpdateUserResponse{
		Status: "ok",
	})
}

func DeleteUser(c *fiber.Ctx) error {
	id, _ := strconv.Atoi(c.Params("id"))
	err := DB.Table("users").Delete(&User{}, id).Error
	if err != nil {
		return err
	}

	return c.JSON(&DeleteUserResponse{
		Status: "ok",
	})
}

func GetDevices(c *fiber.Ctx) error {
	devices := &GetDevicesResponse{}
	err := DB.Table("devices").Scan(&devices.Devices).Error
	if err != nil {
		return err
	}
	return c.JSON(&devices)
}

func GetDevice(c *fiber.Ctx) error {
	id, _ := strconv.Atoi(c.Params("id"))
	device := &GetDeviceResponse{}
	err := DB.Table("devices").Where("id = ?", id).Scan(&device).Error
	if err != nil {
		return err
	}
	return c.JSON(&device)
}

func CreateDevice(c *fiber.Ctx) error {
	device := &CreateDeviceRequest{}
	if err := c.BodyParser(&device); err != nil {
		return err
	}
	err := DB.Table("devices").Create(&device).Error
	if err != nil {
		return err
	}

	return c.JSON(&CreateDeviceResponse{
		Status: "ok",
	})
}

func UpdateDevice(c *fiber.Ctx) error {
	id, _ := strconv.Atoi(c.Params("id"))
	device := &UpdateDeviceRequest{}
	if err := c.BodyParser(&device); err != nil {
		return err
	}
	err := DB.Table("devices").Where("id = ?", id).Updates(&device).Error
	if err != nil {
		return err
	}

	return c.JSON(&UpdateDeviceResponse{
		Status: "ok",
	})
}

func DeleteDevice(c *fiber.Ctx) error {
	id, _ := strconv.Atoi(c.Params("id"))
	err := DB.Table("devices").Delete(&User{}, id).Error
	if err != nil {
		return err
	}

	return c.JSON(&DeleteUserResponse{
		Status: "ok",
	})
}

func GetUserDevice(c *fiber.Ctx) error {
	id, _ := strconv.Atoi(c.Params("id"))
	response := &GetUserDeviceResponse{}
	err := DB.Table("users_devices").Where("user_id = ? AND active", id).Scan(&response).Error
	if err != nil {
		return err
	}
	return c.JSON(&response)
}

func GetDeviceUser(c *fiber.Ctx) error {
	id, _ := strconv.Atoi(c.Params("id"))
	response := &GetUserDeviceResponse{}
	err := DB.Table("users_devices").Where("device_id = ? AND active", id).Scan(&response).Error
	if err != nil {
		return err
	}
	return c.JSON(&response)
}

func CreateUserDevice(c *fiber.Ctx) error {
	userDevice := &CreateUserDeviceRequest{}
	if err := c.BodyParser(&userDevice); err != nil {
		return err
	}

	err := DB.Table("users_devices").Where("device_id = ? AND active", userDevice.DeviceID).Update("active", false).Error
	if err != nil {
		return c.SendStatus(500)
	}
	response := &CreateUserDeviceResponse{
		UserID:   userDevice.UserID,
		DeviceID: userDevice.DeviceID,
		Active:   true,
	}
	err = DB.Table("users_devices").Create(&response).Error
	if err != nil {
		return err
	}
	return c.JSON(&response)

}

func GetReadings(c *fiber.Ctx) error {
	userDevice := &GetReadingsRequest{}
	if err := c.BodyParser(&userDevice); err != nil {
		return err
	}
	response := &GetReadingsResponse{}
	if userDevice.DeviceID != 0 {
		var id uint
		err := DB.Table("users_devices").Select("id").Where("device_id = ? AND active", userDevice.DeviceID).Scan(&id).Error
		if err != nil {
			return err
		}
		err = DB.Table("readings").Where("id = ?", id).Scan(&response.Readings).Error
		if err != nil {
			return err
		}
		return c.JSON(&response)
	}
	if userDevice.UserID != 0 {
		var id uint
		err := DB.Table("users_devices").Select("id").Where("user_id = ?", userDevice.UserID).Scan(&id).Error
		if err != nil {
			return err
		}
		err = DB.Table("readings").Where("id = ?", id).Scan(&response.Readings).Error
		if err != nil {
			return err
		}
		return c.JSON(&response)
	}
	return c.SendStatus(404)
}

func GetReading(c *fiber.Ctx) error {
	id, _ := strconv.Atoi(c.Params("id"))
	reading := &Reading{}
	err := DB.Table("readings").Where("id = ?", id).Scan(&reading).Error
	if err != nil {
		return err
	}
	return c.JSON(&reading)
}

func CreateReading(c *fiber.Ctx) error {
	request := &CreateReadingRequest{}
	if err := c.BodyParser(&request); err != nil {
		return err
	}
	var token string
	err := DB.Table("devices").Select("token").Where("id = ?", request.ID).Scan(token).Error
	if err != nil {
		return err
	}

	if token != request.Token {
		return c.SendStatus(400)
	}

	var id uint
	err = DB.Table("users_devices").Select("id").Where("device_id = ? and active", request.ID).Scan(id).Error
	if err != nil {
		return err
	}

	reading := &Reading{
		UserDeviceID: id,
		Temperature:  request.Temperature,
		Time:         time.Now(),
	}

	err = DB.Table("reading").Create(&reading).Error
	if err != nil {
		return err
	}
	return c.SendStatus(200)
}
