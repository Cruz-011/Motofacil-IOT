package com.motofacil.backend;

import org.springframework.data.jpa.repository.JpaRepository;

public interface LocationRepository extends JpaRepository<Location, Long> {
    Location findTopByOrderByTimestampDesc();
}
