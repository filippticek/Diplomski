package main

import "time"

type User struct {
	Id        uint   `json:"id,omitempty"`
	FirstName string `json:"first_name,omitempty"`
	LastName  string `json:"name,omitempty"`
	Username  string `json:"email,omitempty" gorm:"unique"`
	Password  string `json:"password,omitempty"`
	Role      string `json:"role,omitempty"`
}

type Device struct {
	Id    uint   `json:"id,omitempty"`
	Name  string `json:"name,omitempty"`
	Token string `json:"token,omitempty"`
}

type Reading struct {
	Id           uint      `json:"id,omitempty"`
	UserDeviceID uint      `json:"user_device_id,omitempty"`
	Temperature  float32   `json:"temperature,omitempty"`
	Time         time.Time `json:"time,omitempty"`
}

type UserDevice struct {
	Id       uint `json:"id,omitempty"`
	UserID   uint `json:"user_id,omitempty"`
	DeviceID uint `json:"device_id,omitempty"`
	Active   bool `json:"active,omitempty"`
}

type GetUserResponse struct {
	Id        uint   `json:"id,omitempty"`
	FirstName string `json:"first_name,omitempty"`
	LastName  string `json:"name,omitempty"`
}

type GetUsersResponse struct {
	Users []GetUserResponse `json:"users,omitempty"`
}

type CreateUserRequest struct {
	FirstName string `json:"first_name,omitempty"`
	LastName  string `json:"name,omitempty"`
	Role      string `json:"role,omitempty"`
}

type CreateUserResponse struct {
	Status string `json:"status,omitempty"`
}

type UpdateUserRequest struct {
	Id        uint   `json:"id,omitempty"`
	FirstName string `json:"first_name,omitempty"`
	LastName  string `json:"name,omitempty"`
	Username  string `json:"email,omitempty" gorm:"unique"`
	Password  string `json:"password,omitempty"`
	Role      string `json:"role,omitempty"`
}

type UpdateUserResponse struct {
	Status string `json:"status,omitempty"`
}

type DeleteUserResponse struct {
	Status string `json:"status,omitempty"`
}

type GetDeviceResponse struct {
	Id   uint   `json:"id,omitempty"`
	Name string `json:"name,omitempty"`
}

type GetDevicesResponse struct {
	Devices []GetDeviceResponse `json:"devices,omitempty"`
}

type CreateDeviceRequest struct {
	Id    uint   `json:"id,omitempty"`
	Name  string `json:"name,omitempty"`
	Token string `json:"token,omitempty"`
}

type CreateDeviceResponse struct {
	Status string `json:"status,omitempty"`
}

type UpdateDeviceRequest struct {
	Id    uint   `json:"id,omitempty"`
	Name  string `json:"name,omitempty"`
	Token string `json:"token,omitempty"`
}
type UpdateDeviceResponse struct {
	Status string `json:"status,omitempty"`
}

type DeleteDeviceResponse struct {
	Status string `json:"status,omitempty"`
}

type GetUserDeviceResponse struct {
	Id       uint `json:"id,omitempty"`
	UserID   uint `json:"user_id,omitempty"`
	DeviceID uint `json:"device_id,omitempty"`
	Active   bool `json:"active,omitempty"`
}

type CreateUserDeviceRequest struct {
	UserID   uint `json:"user_id,omitempty"`
	DeviceID uint `json:"device_id,omitempty"`
}

type CreateUserDeviceResponse struct {
	Id       uint `json:"id,omitempty"`
	UserID   uint `json:"user_id,omitempty"`
	DeviceID uint `json:"device_id,omitempty"`
	Active   bool `json:"active,omitempty"`
}

type GetReadingsRequest struct {
	UserID   uint `json:"user_id,omitempty"`
	DeviceID uint `json:"device_id,omitempty"`
}

type GetReadingsResponse struct {
	Readings []Reading `json:"readings,omitempty"`
}

type CreateReadingRequest struct {
	ID          uint    `json:"id,omitempty"`
	Temperature float32 `json:"temperature,omitempty"`
	Token       string  `json:"token,omitempty"`
}
