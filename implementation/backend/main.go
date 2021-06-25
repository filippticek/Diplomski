package main

import (
	"fmt"

	"github.com/gofiber/fiber/v2"
	"github.com/gofiber/fiber/v2/middleware/cors"
	"gorm.io/driver/postgres"
	"gorm.io/gorm"
)

var DB *gorm.DB

func middleware(c *fiber.Ctx) error {
	fmt.Println("Don't mind me!")
	return c.Next()
}

func main() {
	dsn := "host=localhost user=temp password=secret dbname=system port=7654 sslmode=disable"
	db, err := gorm.Open(postgres.Open(dsn), &gorm.Config{})

	if err != nil {
		panic("could not connect to the database")
	}
	DB = db

	app := fiber.New()

	app.Use(cors.New(cors.Config{
		AllowCredentials: true,
	}))

	api := app.Group("/api", middleware)

	// API v1 routes
	v1 := api.Group("/v1", middleware)
	Routes(v1)

	app.Listen(":8000")
}
