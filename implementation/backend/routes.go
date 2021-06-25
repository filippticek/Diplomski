package main

import "github.com/gofiber/fiber/v2"

func Routes(router fiber.Router) {
	router.Get("/user", GetUsers)
	router.Get("/user/:id", GetUser)
	router.Post("/user", CreateUser)
	router.Put("/user/:id", UpdateUser)
	router.Delete("/user/:id", DeleteUser)

	router.Get("/device", GetDevices)
	router.Get("/device/:id", GetDevice)
	router.Post("/device", CreateDevice)
	router.Put("/device/:id", UpdateDevice)
	router.Delete("/device/:id", DeleteDevice)

	router.Get("/user_device/:id", GetUserDevice)
	router.Get("/device_user/:id", GetDeviceUser)
	router.Post("/user_device", CreateUserDevice)
	//router.Put("/user_device/:id", handler)
	//router.Delete("/user_device/:id", handler)

	router.Get("/reading", GetReadings)
	router.Get("/reading/:id", GetReading)
	router.Post("/reading", CreateReading)
	//router.Put("/reading/:id", handler)
	//router.Delete("/reading/:id", handler)
}
