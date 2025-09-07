package com.motofacil.backend;

import org.springframework.web.bind.annotation.*;
import org.springframework.beans.factory.annotation.Autowired;

@RestController
@RequestMapping("/api/location")
public class LocationController {

    @Autowired
    private LocationRepository locationRepository;

    @PostMapping
    public void saveLocation(@RequestBody Location location) {
        locationRepository.save(location);
    }

    @GetMapping("/latest")
    public Location getLatestLocation() {
        return locationRepository.findTopByOrderByTimestampDesc();
    }
}
